/**
 * @file main.c
 * @brief Einfaches Taster-Beispiel mit Polling.
 *
 * LED an PC13 (active-low, Blue Pill).
 * Taster an PA1 gegen GND mit internem Pull-up.
 */

#include "stm32f103x6.h"
#include "gpio.h"

/**
 * @brief Hauptfunktion.
 *
 * Die Funktion wird vom Reset_Handler aufgerufen und kehrt nicht zurueck.
 */
int main(void) {
    /* LED an PC13 wird initialisiert. */
    gpio_init(GPIOC, 13, GPIO_MODE_OUTPUT_2MHZ, GPIO_CNF_OUTPUT_PUSHPULL);
    /* PA0 wird als Debug-Ausgang initialisiert. */
    gpio_init(GPIOA, 0, GPIO_MODE_OUTPUT_10MHZ, GPIO_CNF_OUTPUT_PUSHPULL);
    /* PA1 wird als Taster-Eingang mit internem Pull-up initialisiert. */
    gpio_init(GPIOA, 1, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL);

    /* PC13 auf HIGH bedeutet LED aus (active-low). */
    gpio_set(GPIOC, 13);
    /* PA0 startet auf LOW. */
    gpio_clear(GPIOA, 0);

    while(1) {
        /* LOW entspricht bei Pull-up einem gedrueckten Taster. */
        if(gpio_read(GPIOA, 1) == 0U) {
            gpio_clear(GPIOC, 13); /* LED an (active-low) */
            gpio_set(GPIOA, 0);    /* Debug-Pin an */
        } else {
            gpio_set(GPIOC, 13);   /* LED aus */
            gpio_clear(GPIOA, 0);  /* Debug-Pin aus */
        }
    }

    return 0;
}
