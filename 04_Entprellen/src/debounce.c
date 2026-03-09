/**
 * @file debounce.c
 * @brief Software-Entprellung für mechanische Taster mit Zustandsmaschine
 *
 * Implementierung basierend auf SysTick-Zeitbasis.
 * Nicht-blockierende Zustandsmaschine mit 30ms Entprellzeit.
 */

#include "debounce.h"
#include "systick.h"
#include <stddef.h>

/* Statische Variablen für die Entprellungslogik */
static ButtonState button_state = BTN_STATE_IDLE;
static uint32_t debounce_start_time = 0U;
static uint8_t last_raw_state = 1U;  /* Pull-up: 1 = nicht gedrückt */
static uint16_t bounce_counter = 0U;
static bool debounced_output = false;

/**
 * @brief Initialisiert die Entprellungslogik
 */
void debounce_init(void) {
    button_state = BTN_STATE_IDLE;
    debounce_start_time = 0U;
    last_raw_state = 1U;  /* Pull-up default */
    bounce_counter = 0U;
    debounced_output = false;
}

/**
 * @brief Aktualisiert die Entprellungslogik
 *
 * Zustandsmaschine:
 * 1. IDLE → DEBOUNCE_PRESS wenn Taster gedrückt (raw_state == 0)
 * 2. DEBOUNCE_PRESS → PRESSED nach DEBOUNCE_TIME_MS wenn immer noch gedrückt
 * 3. PRESSED → DEBOUNCE_RELEASE wenn Taster losgelassen (raw_state == 1)
 * 4. DEBOUNCE_RELEASE → IDLE nach DEBOUNCE_TIME_MS wenn immer noch losgelassen
 *
 * Prellen-Ereignisse werden gezählt, wenn sich raw_state ändert,
 * während wir im DEBOUNCE_* Zustand sind.
 */
void debounce_update(uint8_t raw_state) {
    uint32_t current_time = systick_now_ms();
    
    /* Zustandsmaschine */
    switch (button_state) {
        case BTN_STATE_IDLE:
            /* Wenn Taster gedrückt (0 bei Pull-up), starte Entprellung */
            if (raw_state == 0U && last_raw_state == 1U) {
                button_state = BTN_STATE_DEBOUNCE_PRESS;
                debounce_start_time = current_time;
                bounce_counter++;  /* Erstes Prellen-Event */
            }
            break;
            
        case BTN_STATE_DEBOUNCE_PRESS:
            /* Während der Entprellung: raw_state-Änderungen zählen als Prellen */
            if (raw_state != last_raw_state) {
                bounce_counter++;
            }
            
            /* Nach DEBOUNCE_TIME_MS prüfen, ob Zustand stabil ist */
            if ((current_time - debounce_start_time) >= DEBOUNCE_TIME_MS) {
                if (raw_state == 0U) {
                    /* Taster immer noch gedrückt → stabiler PRESSED Zustand */
                    button_state = BTN_STATE_PRESSED;
                    debounced_output = true;
                } else {
                    /* Taster wieder losgelassen → zurück zu IDLE */
                    button_state = BTN_STATE_IDLE;
                }
            }
            break;
            
        case BTN_STATE_PRESSED:
            /* Wenn Taster losgelassen (1 bei Pull-up), starte Entprellung für Release */
            if (raw_state == 1U && last_raw_state == 0U) {
                button_state = BTN_STATE_DEBOUNCE_RELEASE;
                debounce_start_time = current_time;
                bounce_counter++;  /* Erstes Prellen-Event für Release */
            }
            break;
            
        case BTN_STATE_DEBOUNCE_RELEASE:
            /* Während der Entprellung: raw_state-Änderungen zählen als Prellen */
            if (raw_state != last_raw_state) {
                bounce_counter++;
            }
            
            /* Nach DEBOUNCE_TIME_MS prüfen, ob Zustand stabil ist */
            if ((current_time - debounce_start_time) >= DEBOUNCE_TIME_MS) {
                if (raw_state == 1U) {
                    /* Taster immer noch losgelassen → stabiler IDLE Zustand */
                    button_state = BTN_STATE_IDLE;
                    debounced_output = false;
                } else {
                    /* Taster wieder gedrückt → zurück zu PRESSED */
                    button_state = BTN_STATE_PRESSED;
                }
            }
            break;
            
        default:
            /* Sollte nicht vorkommen, aber falls doch: zurück zu IDLE */
            button_state = BTN_STATE_IDLE;
            break;
    }
    
    /* Letzten Rohzustand speichern für nächsten Aufruf */
    last_raw_state = raw_state;
}

/**
 * @brief Gibt den entprellten Tasterzustand zurück
 *
 * @return true  - Taster ist stabil gedrückt
 * @return false - Taster ist stabil nicht gedrückt
 */
bool debounce_get_state(void) {
    return debounced_output;
}

/**
 * @brief Gibt die Anzahl der erkannten Prellen-Ereignisse zurück
 *
 * @return Anzahl der Prellen-Ereignisse seit Initialisierung
 */
uint16_t debounce_get_bounce_count(void) {
    return bounce_counter;
}

/**
 * @brief Gibt den aktuellen Zustand der Zustandsmaschine zurück
 *
 * @return Aktueller ButtonState
 */
ButtonState debounce_get_machine_state(void) {
    return button_state;
}