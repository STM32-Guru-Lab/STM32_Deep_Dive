/**
 * @file system.c
 * @brief System-Initialisierung für STM32F103C6
 * 
 * Diese Datei enthält die Clock-Konfiguration.
 * Standard: HSI 8MHz (nach Reset)
 * Später: HSE + PLL für 72MHz
 */

#include "stm32f10x.h"

/**
 * @brief Minimalste System-Initialisierung
 * 
 * Setzt nur das, was für 8MHz HSI-Betrieb notwendig ist:
 * - HSI als SYSCLK (8MHz)
 * - SYSCLK = HSI = 8MHz
 */
void SystemInit(void) {
    /* 1) HSI einschalten und auf Stabilität warten */
    RCC->CR |= RCC_CR_HSION;
    while(!(RCC->CR & RCC_CR_HSIRDY));

    /* 2) System Clock auf HSI setzen */
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_HSI;
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);

    /* 3) Prescaler = 1 (AHB/APB1/APB2 ohne Division) */
    RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);

    /* 4) SystemCoreClock Variable aktualisieren */
    SystemCoreClock = 8000000U;
}

/**
 * @brief Aktualisiert SystemCoreClock Variable
 * 
 * Diese Funktion sollte aufgerufen werden, wenn sich die Clock-Konfiguration ändert.
 * Aktuell einfach auf 8MHz gesetzt.
 */
void SystemCoreClockUpdate(void) {
    SystemCoreClock = 8000000;  /* HSI 8MHz */
}

/* SystemCoreClock Variable Definition */
uint32_t SystemCoreClock = 8000000;
