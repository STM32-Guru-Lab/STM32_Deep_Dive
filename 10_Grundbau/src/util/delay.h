/**
 * @file delay.h
 * @brief Busy-wait delay functions for STM32F103C6
 * 
 * Simple delay functions based on system clock.
 * CPU-intensive but simple for testing.
 */

#ifndef DELAY_H
#define DELAY_H

#include <stdint.h>

/**
 * @brief Delay for a number of milliseconds (busy-wait)
 * 
 * @param ms Milliseconds to delay
 */
void delay_ms(uint32_t ms);

/**
 * @brief Delay for a number of microseconds (busy-wait)
 * 
 * Note: Not very accurate for short delays due to loop overhead.
 * 
 * @param us Microseconds to delay
 */
void delay_us(uint32_t us);

/**
 * @brief Get current system clock frequency
 * 
 * Helper function for delay calculations.
 * 
 * @return System clock frequency in Hz
 */
uint32_t delay_get_system_clock(void);

/**
 * @brief Calibrate delay loops
 * 
 * Can be used to improve accuracy if needed.
 */
void delay_calibrate(void);

#endif /* DELAY_H */