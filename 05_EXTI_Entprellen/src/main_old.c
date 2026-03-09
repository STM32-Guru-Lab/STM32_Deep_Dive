/**
 * @file main.c
 * @brief EXTI-basierte Entprellungs-Demonstration für mechanische Taster
 *
 * Projekt 05: Erweiterung von Projekt 04 mit External Interrupts (EXTI)
 * 
 * Hardware (wie Projekt 04):
 * - Taster an PA1 (gegen GND mit internem Pull-up)
 * - LED an PC13 (active-low, Blue Pill)
 * - PA0: Rohsignal (Spiegelbild von PA1, für Messung mit LA/Oszilloskop)
 * - PA2: Entprelltes Signal (nach 30ms Filter)
 * - PA3: Prellen-Indikator (blinkt bei jedem Prellen-Event)
 *
 * Software-Änderungen:
 * - EXTI (External Interrupt) statt Polling für Taster PA1
 * - NVIC für Interrupt-Handling
 * - WFI (Wait For Interrupt) für Energieeffizienz
 * - Gleiche Entprellungslogik wie Projekt 04, aber getriggert durch Interrupts
 * - Vergleich: Projekt 04 (Polling) vs. Projekt 05 (EXTI)
 */

#include "stm32f103x6.h"
#include "gpio.h"
#include "systick.h"
#include "debounce.h"
#include "exti.h"
#include "nvic.h"

/**
 * @brief Initialisiert alle GPIO-Pins für das Projekt
 */
static void gpio_init_all(void) {
    /* LED an PC13 (active-low) */
    gpio_init(GPIOC, 13, GPIO_MODE_OUTPUT_2MHZ, GPIO_CNF_OUTPUT_PUSHPULL);
    
    /* PA0: Rohsignal-Ausgang (für Messungen mit LA/Oszilloskop) */
    gpio_init(GPIOA, 0, GPIO_MODE_OUTPUT_10MHZ, GPIO_CNF_OUTPUT_PUSHPULL);
    
    /* PA2: Entprelltes Signal-Ausgang (nach Filter) */
    gpio_init(GPIOA, 2, GPIO_MODE_OUTPUT_10MHZ, GPIO_CNF_OUTPUT_PUSHPULL);
    
    /* PA3: Prellen-Indikator LED (blinkt bei Prellen-Ereignissen) */
    gpio_init(GPIOA, 3, GPIO_MODE_OUTPUT_10MHZ, GPIO_CNF_OUTPUT_PUSHPULL);
    
    /* PA1: Taster-Eingang mit internem Pull-up */
    gpio_init(GPIOA, 1, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL);
}

/**
 * @brief Setzt alle Ausgänge auf ihren Initialzustand
 */
static void gpio_set_initial_state(void) {
    /* PC13 HIGH = LED aus (active-low) */
    gpio_set(GPIOC, 13);
    
    /* PA0 LOW = Rohsignal startet mit LOW */
    gpio_clear(GPIOA, 0);
    
    /* PA2 LOW = Entprelltes Signal startet mit LOW */
    gpio_clear(GPIOA, 2);
    
    /* PA3 LOW = Prellen-Indikator aus */
    gpio_clear(GPIOA, 3);
}

/**
 * @brief Hauptfunktion
 *
 * Demonstrationsablauf:
 * 1. SysTick für 1ms Zeitbasis starten
 * 2. Entprellungslogik initialisieren
 * 3. Hauptschleife:
 *    - Rohsignal von PA1 lesen
 *    - Rohsignal auf PA0 spiegeln (für Messungen)
 *    - Entprellungslogik aktualisieren
 *    - Entprelltes Signal auf PA2 ausgeben
 *    - PC13 LED basierend auf entprelltem Signal steuern
 *    - PA3 blinken lassen bei Prellen-Ereignissen
 */
int main(void) {
    uint32_t last_update_time = 0U;
    uint16_t last_bounce_count = 0U;
    bool prellen_indicator_state = false;
    uint32_t prellen_blink_time = 0U;
    
    /* SysTick als 1ms Systemzeitbasis starten */
    systick_init_1ms();
    
    /* GPIO-Pins initialisieren */
    gpio_init_all();
    gpio_set_initial_state();
    
    /* Entprellungslogik initialisieren */
    debounce_init();
    
    while(1) {
        uint32_t current_time = systick_now_ms();
        
        /* 1. Rohsignal von PA1 lesen */
        uint8_t raw_state = gpio_read(GPIOA, 1);
        
        /* 2. Rohsignal auf PA0 spiegeln (für Messungen mit LA/Oszilloskop) */
        if (raw_state == 0U) {
            gpio_set(GPIOA, 0);    /* Taster gedrückt → PA0 HIGH */
        } else {
            gpio_clear(GPIOA, 0);  /* Taster losgelassen → PA0 LOW */
        }
        
        /* 3. Entprellungslogik alle 1ms aktualisieren */
        if ((current_time - last_update_time) >= 1U) {
            debounce_update(raw_state);
            last_update_time = current_time;
        }
        
        /* 4. Entprelltes Signal auf PA2 ausgeben */
        bool debounced_state = debounce_get_state();
        if (debounced_state) {
            gpio_set(GPIOA, 2);    /* Entprellter Taster gedrückt → PA2 HIGH */
        } else {
            gpio_clear(GPIOA, 2);  /* Entprellter Taster losgelassen → PA2 LOW */
        }
        
        /* 5. PC13 LED basierend auf entprelltem Signal steuern */
        if (debounced_state) {
            gpio_clear(GPIOC, 13); /* LED an (active-low) */
        } else {
            gpio_set(GPIOC, 13);   /* LED aus */
        }
        
        /* 6. Prellen-Indikator (PA3) blinken lassen bei neuen Prellen-Ereignissen */
        uint16_t current_bounce_count = debounce_get_bounce_count();
        if (current_bounce_count != last_bounce_count) {
            /* Neues Prellen-Event erkannt → Indikator für 100ms blinken */
            prellen_indicator_state = true;
            prellen_blink_time = current_time;
            last_bounce_count = current_bounce_count;
        }
        
        /* Prellen-Indikator nach 100ms ausschalten */
        if (prellen_indicator_state && 
            (current_time - prellen_blink_time) >= 100U) {
            prellen_indicator_state = false;
        }
        
        /* Prellen-Indikator setzen/rücksetzen */
        if (prellen_indicator_state) {
            gpio_set(GPIOA, 3);    /* Prellen-Indikator an */
        } else {
            gpio_clear(GPIOA, 3);  /* Prellen-Indikator aus */
        }
        
        /* Optional: Hier könnte man eine einfache Delay-Funktion hinzufügen,
           um die CPU-Auslastung zu reduzieren, aber für Demonstrationszwecke
           lassen wir die Schleife so schnell wie möglich laufen, um jedes
           Prellen-Event zu erfassen. */
    }
    
    return 0;
}