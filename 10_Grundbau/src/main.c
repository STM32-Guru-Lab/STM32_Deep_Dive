/**
 * @file main.c
 * @brief Demonstration of Projekt 10_Grundbau
 * 
 * Basic LED blink with runtime clock configuration.
 * Demonstrates:
 * - Runtime clock selection (HSI 8MHz or HSE 72MHz)
 * - Modular driver structure
 * - Busy-wait delay functions
 */

#include "drivers/system/clock.h"
#include "drivers/gpio/gpio.h"
#include "util/delay.h"
#include <stdint.h>

/* Blue Pill LED is on PC13 (active-low) */
#define LED_PORT    GPIOC
#define LED_PIN     13

/**
 * @brief Simple LED blink demonstration
 */
void blink_demo(void) {
    /* Blink LED 5 times fast */
    for (int i = 0; i < 5; i++) {
        gpio_clear(LED_PORT, LED_PIN);  /* LED on (active-low) */
        delay_ms(100);
        gpio_set(LED_PORT, LED_PIN);    /* LED off */
        delay_ms(100);
    }
    
    /* Blink LED 3 times slow */
    for (int i = 0; i < 3; i++) {
        gpio_clear(LED_PORT, LED_PIN);
        delay_ms(500);
        gpio_set(LED_PORT, LED_PIN);
        delay_ms(500);
    }
}

/**
 * @brief Main function
 */
int main(void) {
    /* Clock configuration structures */
    ClockConfig hsi_config = CLOCK_CONFIG_DEFAULT;      /* HSI 8 MHz */
    
    /* Initialize system clock with HSI (default after reset) */
    if (!clock_init(&hsi_config)) {
        /* If clock init fails, we're stuck - but keep going */
    }
    
    /* Initialize GPIO for LED */
    gpio_init(LED_PORT, LED_PIN, 
              GPIO_MODE_OUTPUT_2MHZ, 
              GPIO_CNF_OUTPUT_PUSHPULL);
    
    /* Turn LED off initially (active-low) */
    gpio_set(LED_PORT, LED_PIN);
    
    /* Demonstration sequence */
    while (1) {
        /* Phase 1: Blink with HSI 8MHz */
        blink_demo();
        delay_ms(1000);
        
        /* Switch to HSE 72MHz */
        if (clock_switch_source(CLOCK_SOURCE_HSE)) {
            /* Phase 2: Blink with HSE 72MHz (same delay, faster execution) */
            blink_demo();
            delay_ms(1000);
            
            /* Switch back to HSI */
            clock_switch_source(CLOCK_SOURCE_HSI);
        } else {
            /* HSE not available (no crystal), just continue with HSI */
        }
        
        /* Longer pause between cycles */
        delay_ms(2000);
    }
    
    return 0;  /* Never reached */
}
