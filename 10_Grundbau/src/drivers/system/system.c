/**
 * @file system.c
 * @brief CMSIS system hooks required by startup code
 */

#include "stm32f103x6.h"

/* Default after reset: HSI = 8 MHz */
uint32_t SystemCoreClock = 8000000U;

void SystemInit(void) {
    /* Ensure HSI is enabled and selected after reset. */
    RCC->CR |= RCC_CR_HSION;
    while ((RCC->CR & RCC_CR_HSIRDY) == 0U) {
    }

    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_HSI;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {
    }

    /* Reset bus prescalers to no division. */
    RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);

    SystemCoreClock = 8000000U;
}

void SystemCoreClockUpdate(void) {
    /*
     * Runtime clock changes are handled in clock.c and keep
     * SystemCoreClock updated. Keep this hook valid for CMSIS callers.
     */
}
