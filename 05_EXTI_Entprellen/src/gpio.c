/**
 * @file gpio.c
 * @brief GPIO Treiber Implementierung
 */

#include "gpio.h"

/**
 * @brief Aktiviert Clock für einen GPIO-Port
 * 
 * Interne Hilfsfunktion fuer gpio_init().
 */
static void gpio_clock_enable(GPIO_TypeDef* port) {
    if(port == GPIOA) {
        RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    } else if(port == GPIOB) {
        RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    } else if(port == GPIOC) {
        RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    }
#ifdef GPIOD
    else if(port == GPIOD) {
        RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;
    }
#endif
#ifdef GPIOE
    else if(port == GPIOE) {
        RCC->APB2ENR |= RCC_APB2ENR_IOPEEN;
    }
#endif
    /* Kurze Verzögerung für Clock-Stabilisierung */
    volatile uint32_t delay = 10;
    while(delay--);
}

void gpio_init(GPIO_TypeDef* port, uint16_t pin, GpioMode mode, GpioConfig config) {
    /* 1. Clock für den Port aktivieren */
    gpio_clock_enable(port);
    
    /* 2. Entscheiden: CRL (Pins 0-7) oder CRH (Pins 8-15) */
    volatile uint32_t* cr_reg;
    uint32_t pin_offset;
    
    if(pin < 8) {
        cr_reg = &port->CRL;
        pin_offset = pin * 4;  /* Jeder Pin: 4 Bits in CRL */
    } else {
        cr_reg = &port->CRH;
        pin_offset = (pin - 8) * 4;  /* Pins 8-15 in CRH */
    }
    
    /* 3. Alte Konfiguration löschen (4 Bits pro Pin) */
    *cr_reg &= ~(0xF << pin_offset);
    
    /* 4. Neue Konfiguration setzen */
    /* STM32F1: Bits [3:2] = CNF, Bits [1:0] = MODE */
    uint32_t config_value = ((uint32_t)config << 2) | (uint32_t)mode;
    *cr_reg |= (config_value << pin_offset);
    
    /* 5. Bei Pull-Up/Pull-Down Input: ODR entsprechend setzen */
    if(config == GPIO_CNF_INPUT_PULL) {
        if(mode == GPIO_MODE_INPUT) {
            /* Pull-Up (ODR=1) oder Pull-Down (ODR=0) */
            /* Default: Pull-Up */
            port->ODR |= (1 << pin);
        }
    }
}

void gpio_set(GPIO_TypeDef* port, uint16_t pin) {
    /* BSRR: Bits 0-15 = Set, Bits 16-31 = Reset */
    port->BSRR = (1 << pin);  /* Atomic set */
}

void gpio_clear(GPIO_TypeDef* port, uint16_t pin) {
    /* BRR: Bits 0-15 = Reset (oder BSRR Bits 16-31) */
    port->BRR = (1 << pin);   /* Atomic clear */
}

void gpio_toggle(GPIO_TypeDef* port, uint16_t pin) {
    /* ODR fuer Toggle (nicht atomar) */
    port->ODR ^= (1 << pin);
}

uint8_t gpio_read(GPIO_TypeDef* port, uint16_t pin) {
    return (port->IDR >> pin) & 0x1;
}
