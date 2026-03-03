/**
 * @file main.c
 * @brief Minimaler Blinky für STM32F103C6
 * 
 * LED an PC13 (active-low auf Blue Pill)
 * Debug-Pin an PA0 für Timing-Messungen am Scope
 */

#include "stm32f10x.h"
#include "gpio.h"

/**
 * @brief Einfache Delay-Funktion (Busy-Wait)
 * 
 * @param iterations Anzahl der Iterationen
 * 
 * Achtung: Nicht zeitlich genau! Nur für Demo.
 * Später durch Timer ersetzen.
 */
static void simple_delay(uint32_t iterations) {
    for(volatile uint32_t i = 0; i < iterations; i++) {
        __asm__("nop");  /* No Operation für stabilere Timing */
    }
}

/**
 * @brief Hauptfunktion
 * 
 * Wird vom Reset_Handler aufgerufen.
 * Gibt nie zurück.
 */
int main(void) {
    /* System ist bereits durch SystemInit() initialisiert (HSI 8MHz) */
    
    /* LED an PC13 initialisieren */
    gpio_init(GPIOC, 13, GPIO_MODE_OUTPUT_2MHZ, GPIO_CNF_OUTPUT_PUSHPULL);
    
    /* Debug-Pin PA0 für Timing-Messungen am Scope */
    gpio_init(GPIOA, 0, GPIO_MODE_OUTPUT_10MHZ, GPIO_CNF_OUTPUT_PUSHPULL);
    
    /* Initialzustand: LED aus (active-low: HIGH = aus) */
    gpio_set(GPIOC, 13);   /* PC13 HIGH = LED aus */
    gpio_clear(GPIOA, 0);  /* PA0 LOW = Debug-Pin aus */
    
    /* Endlosschleife */
    while(1) {
        /* Debug-Pin HIGH (Timing-Marker Start) */
        gpio_set(GPIOA, 0);
        
        /* LED toggeln (PC13) */
        gpio_toggle(GPIOC, 13);
        
        /* Kurze Delay */
        simple_delay(400000);
        
        /* Debug-Pin LOW (Timing-Marker Ende) */
        gpio_clear(GPIOA, 0);
        
        /* Nochmal Delay */
        simple_delay(400000);
    }
    
    /* Wird nie erreicht */
    return 0;
}