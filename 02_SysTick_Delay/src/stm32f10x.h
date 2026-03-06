#ifndef STM32F10X_H
#define STM32F10X_H

#include <stdint.h>

/* System clock variable (defined in system.c) */
extern uint32_t SystemCoreClock;
void SystemInit(void);
void SystemCoreClockUpdate(void);

/* Memory Mapping */
#define PERIPH_BASE       0x40000000UL
#define APB1PERIPH_BASE   PERIPH_BASE
#define APB2PERIPH_BASE   (PERIPH_BASE + 0x10000)
#define AHBPERIPH_BASE    (PERIPH_BASE + 0x20000)

/* RCC */
#define RCC_BASE          (AHBPERIPH_BASE + 0x1000)
typedef struct {
    volatile uint32_t CR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    volatile uint32_t APB2RSTR;
    volatile uint32_t APB1RSTR;
    volatile uint32_t AHBENR;
    volatile uint32_t APB2ENR;
    volatile uint32_t APB1ENR;
    volatile uint32_t BDCR;
    volatile uint32_t CSR;
} RCC_TypeDef;
#define RCC ((RCC_TypeDef *)RCC_BASE)

/* GPIO */
#define GPIOA_BASE        (APB2PERIPH_BASE + 0x0800)
#define GPIOB_BASE        (APB2PERIPH_BASE + 0x0C00)
#define GPIOC_BASE        (APB2PERIPH_BASE + 0x1000)
typedef struct {
    volatile uint32_t CRL;
    volatile uint32_t CRH;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t BRR;
    volatile uint32_t LCKR;
} GPIO_TypeDef;
#define GPIOA ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC ((GPIO_TypeDef *)GPIOC_BASE)

/* FLASH */
#define FLASH_BASE        (AHBPERIPH_BASE + 0x2000)
typedef struct {
    volatile uint32_t ACR;
    volatile uint32_t KEYR;
    volatile uint32_t OPTKEYR;
    volatile uint32_t SR;
    volatile uint32_t CR;
    volatile uint32_t AR;
    volatile uint32_t RESERVED;
    volatile uint32_t OBR;
    volatile uint32_t WRPR;
} FLASH_TypeDef;
#define FLASH ((FLASH_TypeDef *)FLASH_BASE)

/* SCB */
#define SCB_BASE          0xE000ED00UL
typedef struct {
    volatile uint32_t CPUID;
    volatile uint32_t ICSR;
    volatile uint32_t VTOR;
    volatile uint32_t AIRCR;
    volatile uint32_t SCR;
    volatile uint32_t CCR;
    volatile uint32_t SHPR1;
    volatile uint32_t SHPR2;
    volatile uint32_t SHPR3;
    volatile uint32_t SHCSR;
    volatile uint32_t CFSR;
    volatile uint32_t HFSR;
    volatile uint32_t DFSR;
    volatile uint32_t MMFAR;
    volatile uint32_t BFAR;
    volatile uint32_t AFSR;
} SCB_TypeDef;
#define SCB ((SCB_TypeDef *)SCB_BASE)

/* SysTick */
#define SYSTICK_BASE      0xE000E010UL
typedef struct {
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;
} SysTick_TypeDef;
#define SysTick ((SysTick_TypeDef *)SYSTICK_BASE)

/* RCC Register Bits */
#define RCC_CR_HSION      (1 << 0)
#define RCC_CR_HSIRDY     (1 << 1)
#define RCC_CR_HSEON      (1 << 16)
#define RCC_CR_HSERDY     (1 << 17)
#define RCC_CR_PLLON      (1 << 24)
#define RCC_CR_PLLRDY     (1 << 25)

/* CFGR field masks */
#define RCC_CFGR_SW        (3 << 0)   /* System clock switch mask */
#define RCC_CFGR_SWS       (3 << 2)   /* System clock switch status mask */
#define RCC_CFGR_HPRE      (0xF << 4) /* AHB prescaler mask */
#define RCC_CFGR_PPRE1     (0x7 << 8) /* APB1 prescaler mask */
#define RCC_CFGR_PPRE2     (0x7 << 11)/* APB2 prescaler mask */

#define RCC_CFGR_SW_HSI   (0 << 0)
#define RCC_CFGR_SW_HSE   (1 << 0)
#define RCC_CFGR_SW_PLL   (2 << 0)
#define RCC_CFGR_SWS_HSI  (0 << 2)
#define RCC_CFGR_SWS_HSE  (1 << 2)
#define RCC_CFGR_SWS_PLL  (2 << 2)

#define RCC_APB2ENR_IOPAEN  (1 << 2)
#define RCC_APB2ENR_IOPBEN  (1 << 3)
#define RCC_APB2ENR_IOPCEN  (1 << 4)
#define RCC_APB2ENR_IOPDEN  (1 << 5)
#define RCC_APB2ENR_IOPEEN  (1 << 6)

/* FLASH Register Bits */
#define FLASH_ACR_PRFTBE  (1 << 4)
#define FLASH_ACR_LATENCY_0 (0 << 0)
#define FLASH_ACR_LATENCY_1 (1 << 0)
#define FLASH_ACR_LATENCY_2 (2 << 0)

/* SysTick CTRL bits */
#define SysTick_CTRL_ENABLE_Msk      (1UL << 0)
#define SysTick_CTRL_TICKINT_Msk     (1UL << 1)
#define SysTick_CTRL_CLKSOURCE_Msk   (1UL << 2)
#define SysTick_CTRL_COUNTFLAG_Msk   (1UL << 16)

#endif /* STM32F10X_H */
