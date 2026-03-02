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
 * - Flash Wait States (0 für ≤24MHz)
 * - Keine PLL, keine HSE
 * - SYSCLK = HSI = 8MHz
 */
void SystemInit(void) {
    /* 1. FPU deaktivieren (STM32F103 hat keine FPU, aber sicherheitshalber) */
    #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));  /* Set CP10 and CP11 Full Access */
    #endif
    
    /* 2. Flash Wait States für 8MHz (0 Wait States) */
    FLASH->ACR = FLASH_ACR_PRFTBE;  /* Prefetch Buffer enable, 0 Wait States */
    
    /* 3. Reset Clock-Konfiguration (sicherstellen, dass HSI aktiv ist) */
    RCC->CR |= RCC_CR_HSION;        /* HSI einschalten */
    while(!(RCC->CR & RCC_CR_HSIRDY));  /* Warten bis HSI stabil */
    
    /* 4. System Clock auf HSI stellen */
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_HSI;
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);
    
    /* 5. Bus Prescaler auf 1 (keine Division) */
    RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
    
    /* 6. Andere Clock-Quellen ausschalten (sicherheitshalber) */
    RCC->CR &= ~(RCC_CR_HSEON | RCC_CR_PLLON);
    
    /* 7. Peripherie-Clocks für wichtige Peripherie einschalten */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN |   /* GPIOA */
                    RCC_APB2ENR_IOPCEN;    /* GPIOC */
    
    /* 8. SystemCoreClock Variable setzen (8MHz) */
    SystemCoreClock = 8000000;
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