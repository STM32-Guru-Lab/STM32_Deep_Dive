/**
 * @file nvic.h
 * @brief Nested Vectored Interrupt Controller (NVIC) für STM32F103
 * 
 * Konfiguration von Interrupt-Prioritäten und -Aktivierung.
 * Einfache Wrapper-Funktionen für NVIC-Register.
 */

#ifndef NVIC_H
#define NVIC_H

#include <stdint.h>
#include "stm32f103x6.h"

/**
 * @brief Interrupt-Prioritäts-Levels
 * 
 * Niedrigere Zahlen = höhere Priorität (0 = höchste)
 * Bei STM32F103: 4 Bits für Priorität (0-15)
 */
typedef enum {
    NVIC_PRIORITY_HIGHEST = 0,
    NVIC_PRIORITY_HIGH    = 3,
    NVIC_PRIORITY_MEDIUM  = 7,
    NVIC_PRIORITY_LOW     = 11,
    NVIC_PRIORITY_LOWEST  = 15
} NvicPriority;

/**
 * @brief Aktiviert einen Interrupt im NVIC
 * 
 * @param irq_num Interrupt-Nummer (z.B. EXTI1_IRQn aus CMSIS)
 * @param priority Prioritäts-Level (0-15)
 */
void nvic_enable_irq(IRQn_Type irq_num, NvicPriority priority);

/**
 * @brief Deaktiviert einen Interrupt im NVIC
 * 
 * @param irq_num Interrupt-Nummer
 */
void nvic_disable_irq(IRQn_Type irq_num);

/**
 * @brief Setzt ausstehende Interrupt-Flags zurück
 * 
 * @param irq_num Interrupt-Nummer
 */
void nvic_clear_pending(IRQn_Type irq_num);

/**
 * @brief Aktiviert globale Interrupts (CPSIE I)
 */
void nvic_enable_global_interrupts(void);

/**
 * @brief Deaktiviert globale Interrupts (CPSID I)
 */
void nvic_disable_global_interrupts(void);

#endif /* NVIC_H */
