/**
 * @file clock.c
 * @brief System Clock Implementation for STM32F103C6
 * 
 * Runtime-configurable clock system with HSI (8 MHz) and HSE (72 MHz) support.
 */

#include "clock.h"
#include "stm32f103x6.h"
#include <stdbool.h>
#include <stddef.h>

/* Global SystemCoreClock variable (from CMSIS) */
extern uint32_t SystemCoreClock;

/* Static variable to track current configuration */
static ClockConfig current_config = CLOCK_CONFIG_DEFAULT;

/**
 * @brief Configure HSI as system clock (8 MHz)
 */
static bool clock_configure_hsi(void) {
    /* 1. Enable HSI if not already enabled */
    RCC->CR |= RCC_CR_HSION;
    
    /* 2. Wait for HSI to stabilize */
    uint32_t timeout = 1000;  /* Conservative timeout */
    while (!(RCC->CR & RCC_CR_HSIRDY)) {
        if (--timeout == 0) {
            return false;  /* HSI failed to stabilize */
        }
    }
    
    /* 3. Set HSI as system clock source */
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_HSI;
    
    /* 4. Wait for clock switch to complete */
    timeout = 1000;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {
        if (--timeout == 0) {
            return false;  /* Clock switch failed */
        }
    }
    
    /* 5. Configure prescalers for no division */
    RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
    
    /* 6. Disable PLL if it was running */
    RCC->CR &= ~RCC_CR_PLLON;
    
    /* 7. Update configuration */
    current_config.source = CLOCK_SOURCE_HSI;
    current_config.freq_hz = 8000000U;
    
    /* 8. Update global variable */
    SystemCoreClock = 8000000U;
    
    return true;
}

/**
 * @brief Configure HSE with PLL for 72 MHz system clock
 */
static bool clock_configure_hse_72mhz(void) {
    /* 1. Enable HSE */
    RCC->CR |= RCC_CR_HSEON;
    
    /* 2. Wait for HSE to stabilize */
    uint32_t timeout = 1000;
    while (!(RCC->CR & RCC_CR_HSERDY)) {
        if (--timeout == 0) {
            return false;  /* HSE failed to stabilize */
        }
    }
    
    /* 3. Configure PLL:
     *    HSE as PLL source (8 MHz)
     *    PLL multiplier = 9 → 8 MHz * 9 = 72 MHz
     */
    RCC->CFGR &= ~RCC_CFGR_PLLSRC;
    RCC->CFGR |= RCC_CFGR_PLLSRC;      /* HSE as PLL source */
    
    RCC->CFGR &= ~RCC_CFGR_PLLMULL;
    RCC->CFGR |= RCC_CFGR_PLLMULL9;    /* PLL multiplier = 9 */
    
    /* 4. Enable PLL */
    RCC->CR |= RCC_CR_PLLON;
    
    /* 5. Wait for PLL to lock */
    timeout = 2000;
    while (!(RCC->CR & RCC_CR_PLLRDY)) {
        if (--timeout == 0) {
            return false;  /* PLL failed to lock */
        }
    }
    
    /* 6. Configure Flash latency for 72 MHz (2 wait states) */
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_2;  /* 2 wait states for 48-72 MHz */
    
    /* 7. Configure AHB prescaler = 1 (no division) */
    RCC->CFGR &= ~RCC_CFGR_HPRE;
    
    /* 8. Configure APB1 prescaler = 2 (max 36 MHz for APB1) */
    RCC->CFGR &= ~RCC_CFGR_PPRE1;
    RCC->CFGR |= RCC_CFGR_PPRE1_2;      /* APB1 = 36 MHz (72/2) */
    
    /* 9. Configure APB2 prescaler = 1 (72 MHz) */
    RCC->CFGR &= ~RCC_CFGR_PPRE2;
    
    /* 10. Switch to PLL as system clock */
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    
    /* 11. Wait for clock switch to complete */
    timeout = 1000;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {
        if (--timeout == 0) {
            return false;  /* Clock switch failed */
        }
    }
    
    /* 12. Update configuration */
    current_config.source = CLOCK_SOURCE_HSE;
    current_config.freq_hz = 72000000U;
    
    /* 13. Update global variable */
    SystemCoreClock = 72000000U;
    
    return true;
}

/**
 * @brief Initialize system clock with runtime configuration
 */
bool clock_init(const ClockConfig* config) {
    if (config == NULL) {
        return clock_configure_hsi();  /* Default to HSI */
    }
    
    switch (config->source) {
        case CLOCK_SOURCE_HSI:
            return clock_configure_hsi();
            
        case CLOCK_SOURCE_HSE:
            if (config->freq_hz == 72000000U) {
                return clock_configure_hse_72mhz();
            }
            /* Other HSE frequencies could be added here */
            break;
    }
    
    /* Fall back to HSI if configuration is not supported */
    return clock_configure_hsi();
}

/**
 * @brief Initialize system clock with default configuration (HSI 8 MHz)
 */
void clock_init_default(void) {
    clock_configure_hsi();
}

/**
 * @brief Get current system core clock frequency
 */
uint32_t clock_get_core_freq(void) {
    return current_config.freq_hz;
}

/**
 * @brief Update SystemCoreClock variable based on current configuration
 */
void clock_update_system_core_clock(void) {
    SystemCoreClock = current_config.freq_hz;
}

/**
 * @brief Check if HSE is stable and ready
 */
bool clock_hse_is_ready(void) {
    return (RCC->CR & RCC_CR_HSERDY) != 0;
}

/**
 * @brief Check if HSI is stable and ready
 */
bool clock_hsi_is_ready(void) {
    return (RCC->CR & RCC_CR_HSIRDY) != 0;
}

/**
 * @brief Switch clock source at runtime
 */
bool clock_switch_source(ClockSource source) {
    ClockConfig new_config = current_config;
    new_config.source = source;
    
    return clock_init(&new_config);
}