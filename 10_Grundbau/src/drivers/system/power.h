/**
 * @file power.h
 * @brief Power Management for STM32F103C6
 * 
 * Provides functions for entering low-power modes.
 */

#ifndef POWER_H
#define POWER_H

#include <stdint.h>

/**
 * @brief Power modes available on STM32F103
 */
typedef enum {
    POWER_MODE_SLEEP,    /**< Sleep mode (CPU stopped, peripherals running) */
    POWER_MODE_STOP,     /**< Stop mode (all clocks stopped) */
    POWER_MODE_STANDBY   /**< Standby mode (lowest power, wakeup resets MCU) */
} PowerMode;

/**
 * @brief Enter sleep mode (WFI - Wait For Interrupt)
 * 
 * CPU stops execution until an interrupt occurs.
 * Peripherals continue running.
 */
void power_enter_sleep(void);

/**
 * @brief Enter sleep mode (WFE - Wait For Event)
 * 
 * CPU stops execution until an event occurs.
 */
void power_enter_sleep_event(void);

/**
 * @brief Enter stop mode
 * 
 * All clocks are stopped, but RAM and registers retain their values.
 * Wakeup sources: EXTI lines, RTC alarm, etc.
 */
void power_enter_stop(void);

/**
 * @brief Enter standby mode
 * 
 * Lowest power mode. RAM content is lost.
 * Wakeup resets the MCU (like a power-on reset).
 */
void power_enter_standby(void);

/**
 * @brief Enable/disable sleep-on-exit feature
 * 
 * When enabled, CPU returns to sleep after ISR execution.
 * 
 * @param enable true to enable, false to disable
 */
void power_set_sleep_on_exit(bool enable);

/**
 * @brief Check if the system is in sleep mode
 * 
 * @return true if in sleep mode, false otherwise
 */
bool power_is_in_sleep(void);

#endif /* POWER_H */