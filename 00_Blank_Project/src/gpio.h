/**
 * @file gpio.h
 * @brief GPIO Treiber für STM32F103 (CMSIS basiert)
 * 
 * Keine HAL, nur strukturierte Registerzugriffe.
 * BSRR statt ODR für atomare Operationen.
 */

#ifndef GPIO_H
#define GPIO_H

#include "stm32f10x.h"

/**
 * @brief GPIO Betriebsmodi (MODE Bits)
 */
typedef enum {
    GPIO_MODE_INPUT       = 0x00,  /* Input mode */
    GPIO_MODE_OUTPUT_10MHZ = 0x01, /* Output mode, 10MHz */
    GPIO_MODE_OUTPUT_2MHZ  = 0x02, /* Output mode, 2MHz */
    GPIO_MODE_OUTPUT_50MHZ = 0x03  /* Output mode, 50MHz */
} GpioMode;

/**
 * @brief GPIO Konfiguration (CNF Bits)
 */
typedef enum {
    /* Input mode configurations */
    GPIO_CNF_INPUT_ANALOG    = 0x00, /* Analog input */
    GPIO_CNF_INPUT_FLOATING  = 0x01, /* Floating input (danger!) */
    GPIO_CNF_INPUT_PULL      = 0x02, /* Input with pull-up/pull-down */
    
    /* Output mode configurations */
    GPIO_CNF_OUTPUT_PUSHPULL = 0x00, /* Output push-pull */
    GPIO_CNF_OUTPUT_OPENDRAIN = 0x01, /* Output open-drain */
    GPIO_CNF_AF_PUSHPULL     = 0x02, /* Alternate function push-pull */
    GPIO_CNF_AF_OPENDRAIN    = 0x03  /* Alternate function open-drain */
} GpioConfig;

/**
 * @brief Initialisiert einen GPIO-Pin
 * 
 * @param port GPIO Port (GPIOA, GPIOB, GPIOC, etc.)
 * @param pin Pin-Nummer (0-15)
 * @param mode Betriebsmodus
 * @param config Konfiguration
 */
void gpio_init(GPIO_TypeDef* port, uint16_t pin, GpioMode mode, GpioConfig config);

/**
 * @brief Setzt einen Pin auf HIGH (atomic mit BSRR)
 */
void gpio_set(GPIO_TypeDef* port, uint16_t pin);

/**
 * @brief Setzt einen Pin auf LOW (atomic mit BRR)
 */
void gpio_clear(GPIO_TypeDef* port, uint16_t pin);

/**
 * @brief Toggelt einen Pin (mit ODR, da BSRR kein Toggle kann)
 * Achtung: Nicht ISR-sicher! Nur im gleichen Thread verwenden.
 */
void gpio_toggle(GPIO_TypeDef* port, uint16_t pin);

/**
 * @brief Liest den Zustand eines Pins
 * 
 * @return 0 wenn LOW, 1 wenn HIGH
 */
uint8_t gpio_read(GPIO_TypeDef* port, uint16_t pin);

#endif /* GPIO_H */