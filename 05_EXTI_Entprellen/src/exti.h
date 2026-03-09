/**
 * @file exti.h
 * @brief External Interrupt (EXTI) Treiber für STM32F103
 * 
 * Konfiguration von EXTI-Lines für GPIO-Pins
 * Interrupt-Handling für Taster-Eingänge
 * 
 * Hardware: Taster an PA1 → EXTI_Line1
 */

#ifndef EXTI_H
#define EXTI_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief EXTI-Line Konfiguration
 */
typedef enum {
    EXTI_TRIGGER_RISING,     ///< Interrupt bei steigender Flanke
    EXTI_TRIGGER_FALLING,    ///< Interrupt bei fallender Flanke
    EXTI_TRIGGER_BOTH        ///< Interrupt bei beiden Flanken
} ExtiTrigger;

/**
 * @brief Initialisiert EXTI für einen GPIO-Pin
 * 
 * @param port GPIO Port (GPIOA, GPIOB, ...)
 * @param pin Pin-Nummer (0-15)
 * @param trigger Auslöser-Typ (steigend, fallend, beide)
 * @return true bei Erfolg, false bei Fehler
 */
bool exti_init(uint32_t port, uint16_t pin, ExtiTrigger trigger);

/**
 * @brief Aktiviert EXTI-Interrupt für die konfigurierte Line
 */
void exti_enable_interrupt(void);

/**
 * @brief Deaktiviert EXTI-Interrupt für die konfigurierte Line
 */
void exti_disable_interrupt(void);

/**
 * @brief Löscht ausstehende EXTI-Interrupt Flags
 */
void exti_clear_pending(void);

/**
 * @brief Prüft ob EXTI-Interrupt ausgelöst wurde
 * 
 * @return true wenn Interrupt-Flag gesetzt
 */
bool exti_is_pending(void);

#endif /* EXTI_H */
