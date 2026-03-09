/**
 * @file exti.c
 * @brief External Interrupt (EXTI) Treiber für STM32F103
 * 
 * Implementierung der EXTI-Konfiguration für Taster PA1.
 * Nutzt AFIO (Alternate Function I/O) für GPIO-zu-EXTI Mapping.
 */

#include "exti.h"
#include "stm32f103x6.h"
#include "gpio.h"
#include <stdbool.h>

/* Statische Variablen für aktuell konfigurierte EXTI-Line */
static uint32_t configured_line = 0;
static uint32_t configured_port = 0;

/**
 * @brief Initialisiert EXTI für einen GPIO-Pin
 */
bool exti_init(uint32_t port, uint16_t pin, ExtiTrigger trigger) {
    /* Parameter-Validierung */
    if (pin > 15) {
        return false;
    }
    
    /* Port auf GPIOA, GPIOB, etc. prüfen */
    if (port != (uint32_t)GPIOA && port != (uint32_t)GPIOB && 
        port != (uint32_t)GPIOC) {
        return false;
    }
    
    /* Speichere Konfiguration */
    configured_line = (1U << pin);
    configured_port = port;
    
    /* 1. AFIO Clock aktivieren (für EXTI Konfiguration benötigt) */
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    
    /* 2. GPIO-Pin zu EXTI-Line mappen über AFIO->EXTICR[] */
    uint8_t exti_cr_index = pin / 4U;
    uint8_t exti_cr_position = (pin % 4U) * 4U;
    
    /* Alten Wert löschen für diesen Pin */
    AFIO->EXTICR[exti_cr_index] &= ~(0xFU << exti_cr_position);
    
    /* Neuen Port zuweisen */
    if (port == (uint32_t)GPIOA) {
        AFIO->EXTICR[exti_cr_index] |= (0x0U << exti_cr_position);
    } else if (port == (uint32_t)GPIOB) {
        AFIO->EXTICR[exti_cr_index] |= (0x1U << exti_cr_position);
    } else if (port == (uint32_t)GPIOC) {
        AFIO->EXTICR[exti_cr_index] |= (0x2U << exti_cr_position);
    }
    
    /* 3. EXTI Trigger-Konfiguration */
    switch (trigger) {
        case EXTI_TRIGGER_RISING:
            EXTI->RTSR |= configured_line;   /* Rising Trigger selektieren */
            EXTI->FTSR &= ~configured_line;  /* Falling Trigger deaktivieren */
            break;
            
        case EXTI_TRIGGER_FALLING:
            EXTI->RTSR &= ~configured_line;  /* Rising Trigger deaktivieren */
            EXTI->FTSR |= configured_line;   /* Falling Trigger selektieren */
            break;
            
        case EXTI_TRIGGER_BOTH:
            EXTI->RTSR |= configured_line;   /* Rising Trigger selektieren */
            EXTI->FTSR |= configured_line;   /* Falling Trigger selektieren */
            break;
    }
    
    /* 4. Interrupt-Maske für diese Line aktivieren */
    EXTI->IMR |= configured_line;
    
    return true;
}

/**
 * @brief Aktiviert EXTI-Interrupt für die konfigurierte Line
 * 
 * Muss nach NVIC-Konfiguration aufgerufen werden.
 */
void exti_enable_interrupt(void) {
    if (configured_line == 0) {
        return; /* Keine Line konfiguriert */
    }
    
    /* EXTI-Interrupt für diese Line aktivieren */
    EXTI->IMR |= configured_line;
    
    /* Interrupt-Flag löschen (falls schon gesetzt) */
    exti_clear_pending();
}

/**
 * @brief Deaktiviert EXTI-Interrupt für die konfigurierte Line
 */
void exti_disable_interrupt(void) {
    if (configured_line == 0) {
        return;
    }
    
    /* EXTI-Interrupt für diese Line deaktivieren */
    EXTI->IMR &= ~configured_line;
}

/**
 * @brief Löscht ausstehende EXTI-Interrupt Flags
 */
void exti_clear_pending(void) {
    if (configured_line == 0) {
        return;
    }
    
    /* Interrupt-Flag für diese Line löschen (durch Schreiben 1) */
    EXTI->PR = configured_line;
}

/**
 * @brief Prüft ob EXTI-Interrupt ausgelöst wurde
 */
bool exti_is_pending(void) {
    if (configured_line == 0) {
        return false;
    }
    
    /* Prüfe ob Interrupt-Flag für diese Line gesetzt ist */
    return (EXTI->PR & configured_line) != 0;
}

