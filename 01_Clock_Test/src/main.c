/**
 * @file main.c
 * @brief Clock-Test: HSI (~8 MHz) auf MCO (PA8) ausgeben
 *
 * Messpunkt:
 *   - PA8 (MCO)
 */

#include "stm32f10x.h"

int main(void) {
    /* GPIOA + AFIO Clock aktivieren (PA8 als Alternate Function Output). */
    RCC->APB2ENR |= (1U << 0);            /* AFIOEN */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;   /* IOPAEN */

    /* PA8: MODE=11 (50 MHz), CNF=10 (AF Push-Pull). */
    GPIOA->CRH &= ~(0xFU << 0);           /* Bits fuer Pin 8 loeschen */
    GPIOA->CRH |=  (0xBU << 0);           /* 1011b */

    /* MCO Quelle = HSI (typ. 8 MHz), RCC_CFGR MCO[26:24] = 101. */
    RCC->CFGR &= ~(0x7U << 24);
    RCC->CFGR |=  (0x5U << 24);

    while (1) {
        __asm__("nop");
    }
}
