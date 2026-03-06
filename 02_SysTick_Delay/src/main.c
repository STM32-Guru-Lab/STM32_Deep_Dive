/**
 * @file main.c
 * @brief Blink-Beispiel mit zentraler SysTick-Zeitbasis.
 *
 * LED an PC13 (active-low, Blue Pill).
 */

#include "stm32f10x.h"
#include "gpio.h"
#include "systick.h"

/**
 * @brief Hauptfunktion.
 *
 * Die Funktion wird vom Reset_Handler aufgerufen und kehrt nicht zurueck.
 */
int main(void) {
    /* SysTick wird als 1-ms-Systemzeitbasis gestartet. */
    systick_init_1ms();

    /* LED an PC13 wird initialisiert. */
    gpio_init(GPIOC, 13, GPIO_MODE_OUTPUT_2MHZ, GPIO_CNF_OUTPUT_PUSHPULL);
    /* PA0 wird als Debug-Ausgang initialisiert. */
    gpio_init(GPIOA, 0, GPIO_MODE_OUTPUT_10MHZ, GPIO_CNF_OUTPUT_PUSHPULL);

    /* PC13 auf HIGH bedeutet LED aus (active-low). */
    gpio_set(GPIOC, 13);
    /* PA0 startet auf LOW. */
    gpio_clear(GPIOA, 0);

    while(1) {
        gpio_set(GPIOA, 0);
        gpio_toggle(GPIOC, 13);
        systick_delay_ms(500U);

        gpio_clear(GPIOA, 0);
        systick_delay_ms(500U);
    }

    return 0;
}
