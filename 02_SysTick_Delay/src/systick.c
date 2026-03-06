/**
 * @file systick.c
 * @brief Zentrale Zeitbasis auf Basis von SysTick.
 */

#include "systick.h"
#include "stm32f10x.h"

/* Der Zaehler enthaelt die vergangene Zeit in Millisekunden. */
static volatile uint32_t g_systick_ms = 0U;

/**
 * @brief SysTick Interrupt-Handler.
 *
 * Der Handler wird alle 1 ms aufgerufen und erhoeht den Millisekundenzaehler.
 */
void SysTick_Handler(void) {
    g_systick_ms++;
}

/**
 * @brief Initialisiert SysTick fuer einen 1-ms-Interrupt.
 *
 * Die Funktion aktiviert SysTick mit Core-Clock als Taktquelle.
 */
void systick_init_1ms(void) {
    SystemCoreClockUpdate();

    SysTick->CTRL = 0U;
    SysTick->LOAD = (SystemCoreClock / 1000U) - 1U;
    SysTick->VAL  = 0U;
    g_systick_ms = 0U;

    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_TICKINT_Msk |
                    SysTick_CTRL_ENABLE_Msk;
}

/**
 * @brief Liefert die aktuelle Systemzeit in Millisekunden.
 */
uint32_t systick_now_ms(void) {
    return g_systick_ms;
}

/**
 * @brief Blockiert fuer die angegebene Zeit in Millisekunden.
 *
 * Die Funktion basiert auf der 1-ms-Systemzeit.
 */
void systick_delay_ms(uint32_t delay_ms) {
    uint32_t start_ms = systick_now_ms();
    while((systick_now_ms() - start_ms) < delay_ms) {
        /* Die Schleife wartet bis die Zielzeit erreicht ist. */
    }
}

/**
 * @brief Prueft, ob ein periodisches Zeitfenster abgelaufen ist.
 *
 * @param last_ms Letzter Zeitstempel der Aufgabe.
 * @param period_ms Gewuenschte Periode in Millisekunden.
 *
 * @return 1 wenn die Periode abgelaufen ist, sonst 0.
 */
uint8_t systick_period_elapsed(uint32_t *last_ms, uint32_t period_ms) {
    uint32_t now_ms = systick_now_ms();

    if((now_ms - *last_ms) >= period_ms) {
        *last_ms = now_ms;
        return 1U;
    }

    return 0U;
}
