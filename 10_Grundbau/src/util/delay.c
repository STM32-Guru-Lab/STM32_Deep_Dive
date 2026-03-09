/**
 * @file delay.c
 * @brief Busy-wait delay implementation for STM32F103C6
 */

#include "delay.h"
#include <stdint.h>

/* External SystemCoreClock variable from CMSIS */
extern uint32_t SystemCoreClock;

/* Calibration factor - can be adjusted for accuracy */
static uint32_t cycles_per_ms = 8000;  /* Default for 8 MHz */

/**
 * @brief Initialize delay system
 * 
 * Calculates cycles based on SystemCoreClock.
 */
static void delay_init(void) {
    /* Update cycles per millisecond based on system clock */
    cycles_per_ms = SystemCoreClock / 1000;
    
    /* Ensure minimum value */
    if (cycles_per_ms < 1) {
        cycles_per_ms = 1;
    }
}

/**
 * @brief Delay for a number of milliseconds (busy-wait)
 */
void delay_ms(uint32_t ms) {
    static uint8_t initialized = 0;
    
    /* Initialize on first call */
    if (!initialized) {
        delay_init();
        initialized = 1;
    }
    
    /* Calculate total cycles needed */
    uint32_t total_cycles = ms * cycles_per_ms;
    
    /* Simple busy-wait loop */
    for (uint32_t i = 0; i < total_cycles; i++) {
        __asm__ volatile ("nop");  /* No-operation instruction */
    }
}

/**
 * @brief Delay for a number of microseconds (busy-wait)
 * 
 * Note: Less accurate for very short delays.
 */
void delay_us(uint32_t us) {
    static uint8_t initialized = 0;
    
    /* Initialize on first call */
    if (!initialized) {
        delay_init();
        initialized = 1;
    }
    
    /* Calculate total cycles needed (convert us to cycles) */
    uint32_t total_cycles = (us * cycles_per_ms) / 1000;
    
    /* Ensure minimum delay (loop overhead) */
    if (total_cycles < 10) {
        total_cycles = 10;
    }
    
    /* Simple busy-wait loop */
    for (uint32_t i = 0; i < total_cycles; i++) {
        __asm__ volatile ("nop");
    }
}

/**
 * @brief Get current system clock frequency
 */
uint32_t delay_get_system_clock(void) {
    return SystemCoreClock;
}

/**
 * @brief Calibrate delay loops
 * 
 * This is a placeholder for future calibration.
 * Could be implemented with a hardware timer.
 */
void delay_calibrate(void) {
    /* Re-initialize with current SystemCoreClock */
    delay_init();
}