/**
 * @file clock.h
 * @brief System Clock Configuration for STM32F103C6
 * 
 * Runtime-configurable clock system with HSI (8 MHz) and HSE (72 MHz) support.
 * Uses PLL for HSE to achieve 72 MHz system clock.
 */

#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Available clock sources
 */
typedef enum {
    CLOCK_SOURCE_HSI = 0,    /**< Internal 8 MHz RC oscillator */
    CLOCK_SOURCE_HSE         /**< External 8 MHz crystal with PLL to 72 MHz */
} ClockSource;

/**
 * @brief Clock configuration structure
 */
typedef struct {
    ClockSource source;      /**< Clock source (HSI or HSE) */
    uint32_t freq_hz;        /**< Desired system frequency in Hz */
} ClockConfig;

/**
 * @brief Default clock configuration (HSI 8 MHz)
 */
#define CLOCK_CONFIG_DEFAULT {CLOCK_SOURCE_HSI, 8000000U}

/**
 * @brief HSE with PLL configuration (72 MHz)
 */
#define CLOCK_CONFIG_HSE_72MHZ {CLOCK_SOURCE_HSE, 72000000U}

/**
 * @brief Initialize system clock with runtime configuration
 * 
 * @param config Clock configuration (source and frequency)
 * @return true if successful, false if configuration failed
 */
bool clock_init(const ClockConfig* config);

/**
 * @brief Initialize system clock with default configuration (HSI 8 MHz)
 */
void clock_init_default(void);

/**
 * @brief Get current system core clock frequency
 * 
 * @return Current system clock frequency in Hz
 */
uint32_t clock_get_core_freq(void);

/**
 * @brief Update SystemCoreClock variable based on current configuration
 * 
 * This should be called after any clock configuration change.
 */
void clock_update_system_core_clock(void);

/**
 * @brief Check if HSE is stable and ready
 * 
 * @return true if HSE is stable, false otherwise
 */
bool clock_hse_is_ready(void);

/**
 * @brief Check if HSI is stable and ready
 * 
 * @return true if HSI is stable, false otherwise
 */
bool clock_hsi_is_ready(void);

/**
 * @brief Switch clock source at runtime
 * 
 * @param source New clock source
 * @return true if switch successful, false otherwise
 */
bool clock_switch_source(ClockSource source);

#endif /* CLOCK_H */