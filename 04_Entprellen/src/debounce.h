/**
 * @file debounce.h
 * @brief Software-Entprellung für mechanische Taster mit Zustandsmaschine
 *
 * Implementierung einer nicht-blockierenden Entprellungslogik
 * basierend auf SysTick-Zeitbasis (1ms).
 * 
 * Merkmale:
 * - Zustandsmaschine mit DEBOUNCE_TIME_MS (konfigurierbar)
 * - Zählt Prellen-Ereignisse für Debug/Statistik
 * - Liefert entprellten Tasterzustand (gedrückt/nicht gedrückt)
 * - Nicht-blockierend (regelmäßig debounce_update() aufrufen)
 */

#ifndef DEBOUNCE_H
#define DEBOUNCE_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Konfigurierbare Entprellzeit in Millisekunden
 * 
 * Basierend auf Messungen: ~20ms Prellen + 10ms Sicherheitsmarge
 * Kann angepasst werden für verschiedene Taster
 */
#define DEBOUNCE_TIME_MS 30U

/**
 * @brief Zustände der Entprellungs-Zustandsmaschine
 */
typedef enum {
    BTN_STATE_IDLE,           ///< Taster nicht gedrückt, stabil
    BTN_STATE_DEBOUNCE_PRESS, ///< Potentieller Tastendruck, warte auf Stabilisierung
    BTN_STATE_PRESSED,        ///< Taster stabil gedrückt
    BTN_STATE_DEBOUNCE_RELEASE ///< Potentielles Loslassen, warte auf Stabilisierung
} ButtonState;

/**
 * @brief Initialisiert die Entprellungslogik
 * 
 * Muss einmalig vor Nutzung aufgerufen werden.
 * Setzt alle internen Variablen auf Initialwerte.
 */
void debounce_init(void);

/**
 * @brief Aktualisiert die Entprellungslogik (regelmäßig aufrufen)
 * 
 * Diese Funktion sollte regelmäßig aufgerufen werden (z.B. alle 1ms).
 * Sie liest den aktuellen Rohzustand des Tasters und verarbeitet
 * die Entprellungs-Zustandsmaschine.
 * 
 * @param raw_state Rohzustand des Tasters (0 = gedrückt, 1 = nicht gedrückt bei Pull-up)
 */
void debounce_update(uint8_t raw_state);

/**
 * @brief Gibt den entprellten Tasterzustand zurück
 * 
 * @return true  - Taster ist stabil gedrückt
 * @return false - Taster ist stabil nicht gedrückt
 */
bool debounce_get_state(void);

/**
 * @brief Gibt die Anzahl der erkannten Prellen-Ereignisse zurück
 * 
 * Zählt jeden Zustandswechsel im Rohsignal, bevor die
 * Entprellungszeit abgelaufen ist.
 * 
 * @return Anzahl der Prellen-Ereignisse seit Initialisierung
 */
uint16_t debounce_get_bounce_count(void);

/**
 * @brief Gibt den aktuellen Zustand der Zustandsmaschine zurück
 * 
 * Für Debugging und Visualisierung.
 * 
 * @return Aktueller ButtonState
 */
ButtonState debounce_get_machine_state(void);

#endif /* DEBOUNCE_H */