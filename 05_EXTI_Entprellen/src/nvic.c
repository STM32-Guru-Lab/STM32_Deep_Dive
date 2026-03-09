/**
 * @file nvic.c
 * @brief Nested Vectored Interrupt Controller (NVIC) für STM32F103
 * 
 * Implementierung der NVIC-Funktionen für Interrupt-Handling.
 * Nutzt CMSIS-Funktionen für atomare Operationen.
 */

#include "nvic.h"
#include "stm32f103x6.h"

/**
 * @brief Aktiviert einen Interrupt im NVIC
 */
void nvic_enable_irq(IRQn_Type irq_num, NvicPriority priority) {
    /* Priorität setzen (4 Bits für STM32F103) */
    NVIC_SetPriority(irq_num, priority);
    
    /* Interrupt aktivieren */
    NVIC_EnableIRQ(irq_num);
    
    /* Ausstehende Flags löschen */
    NVIC_ClearPendingIRQ(irq_num);
}

/**
 * @brief Deaktiviert einen Interrupt im NVIC
 */
void nvic_disable_irq(IRQn_Type irq_num) {
    /* Interrupt deaktivieren */
    NVIC_DisableIRQ(irq_num);
    
    /* Ausstehende Flags löschen */
    NVIC_ClearPendingIRQ(irq_num);
}

/**
 * @brief Setzt ausstehende Interrupt-Flags zurück
 */
void nvic_clear_pending(IRQn_Type irq_num) {
    NVIC_ClearPendingIRQ(irq_num);
}

/**
 * @brief Aktiviert globale Interrupts (CPSIE I)
 * 
 * Assembler-Instruktion: CPSIE I
 */
void nvic_enable_global_interrupts(void) {
    __enable_irq();
}

/**
 * @brief Deaktiviert globale Interrupts (CPSID I)
 * 
 * Assembler-Instruktion: CPSID I
 */
void nvic_disable_global_interrupts(void) {
    __disable_irq();
}