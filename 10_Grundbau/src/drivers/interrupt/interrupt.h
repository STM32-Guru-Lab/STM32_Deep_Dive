/**
 * @file interrupt.h
 * @brief Unified Interrupt API for STM32F103C6
 * 
 * Combined EXTI and NVIC management with callback support.
 */

#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Interrupt priority levels (0 = highest, 15 = lowest)
 */
typedef enum {
    IRQ_PRIORITY_HIGHEST = 0,
    IRQ_PRIORITY_HIGH = 4,
    IRQ_PRIORITY_MEDIUM = 8,
    IRQ_PRIORITY_LOW = 12,
    IRQ_PRIORITY_LOWEST = 15
} IrqPriority;

/**
 * @brief EXTI trigger modes
 */
typedef enum {
    EXTI_TRIGGER_RISING,     /**< Interrupt on rising edge */
    EXTI_TRIGGER_FALLING,    /**< Interrupt on falling edge */
    EXTI_TRIGGER_BOTH        /**< Interrupt on both edges */
} ExtiTrigger;

/**
 * @brief EXTI line callback function type
 */
typedef void (*ExtiCallback)(uint16_t line);

/**
 * @brief Initialize interrupt subsystem
 * 
 * Must be called before any interrupt configuration.
 */
void interrupt_init(void);

/**
 * @brief Configure EXTI for a GPIO pin
 * 
 * @param port GPIO port (GPIOA, GPIOB, etc.)
 * @param pin Pin number (0-15)
 * @param trigger Trigger mode
 * @param priority Interrupt priority
 * @return true if successful, false on error
 */
bool interrupt_exti_configure(uint32_t port, uint16_t pin, 
                              ExtiTrigger trigger, IrqPriority priority);

/**
 * @brief Register callback for EXTI line
 * 
 * @param line EXTI line (0-15)
 * @param callback Function to call when interrupt occurs
 */
void interrupt_exti_register_callback(uint16_t line, ExtiCallback callback);

/**
 * @brief Enable EXTI interrupt for specific line
 * 
 * @param line EXTI line (0-15)
 */
void interrupt_exti_enable(uint16_t line);

/**
 * @brief Disable EXTI interrupt for specific line
 * 
 * @param line EXTI line (0-15)
 */
void interrupt_exti_disable(uint16_t line);

/**
 * @brief Clear pending EXTI interrupt flag
 * 
 * @param line EXTI line (0-15)
 */
void interrupt_exti_clear_pending(uint16_t line);

/**
 * @brief Check if EXTI interrupt is pending
 * 
 * @param line EXTI line (0-15)
 * @return true if interrupt is pending, false otherwise
 */
bool interrupt_exti_is_pending(uint16_t line);

/**
 * @brief Enable global interrupts
 */
void interrupt_enable_global(void);

/**
 * @brief Disable global interrupts
 */
void interrupt_disable_global(void);

/**
 * @brief Set interrupt priority
 * 
 * @param irq_number NVIC IRQ number
 * @param priority Priority level (0-15)
 */
void interrupt_set_priority(int32_t irq_number, IrqPriority priority);

/**
 * @brief Enable NVIC interrupt
 * 
 * @param irq_number NVIC IRQ number
 */
void interrupt_enable(int32_t irq_number);

/**
 * @brief Disable NVIC interrupt
 * 
 * @param irq_number NVIC IRQ number
 */
void interrupt_disable(int32_t irq_number);

/**
 * @brief Clear pending NVIC interrupt
 * 
 * @param irq_number NVIC IRQ number
 */
void interrupt_clear_pending(int32_t irq_number);

#endif /* INTERRUPT_H */