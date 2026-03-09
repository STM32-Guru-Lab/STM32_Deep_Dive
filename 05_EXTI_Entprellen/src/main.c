/**
 * @file main_exti.c
 * @brief EXTI-basierte Entprellungs-Demonstration für mechanische Taster
 *
 * Projekt 05: Erweiterung von Projekt 04 mit External Interrupts (EXTI)
 * 
 * Hardware (wie Projekt 04):
 * - Taster an PA1 (gegen GND mit internem Pull-up)
 * - LED an PC13 (active-low, Blue Pill)
 * - PA0: Rohsignal (Spiegelbild von PA1, für Messung mit LA/Oszilloskop)
 * - PA2: Entprelltes Signal (nach 30ms Filter)
 * - PA3: Prellen-Indikator (blinkt bei jedem Prellen-Event)
 *
 * Software-Änderungen:
 * - EXTI (External Interrupt) statt Polling für Taster PA1
 * - NVIC für Interrupt-Handling
 * - WFI (Wait For Interrupt) für Energieeffizienz
 * - Gleiche Entprellungslogik wie Projekt 04, aber getriggert durch Interrupts
 * - Vergleich: Projekt 04 (Polling) vs. Projekt 05 (EXTI)
 */

#include "stm32f103x6.h"
#include "gpio.h"
#include "systick.h"
#include "debounce.h"
#include "exti.h"
#include "nvic.h"

/* Globale Variablen für Interrupt-Kommunikation */
static volatile uint8_t exti_triggered = 0;
static volatile uint32_t last_exti_time = 0;

/**
 * @brief EXTI1_IRQHandler - Interrupt Service Routine für EXTI Line1
 * 
 * Wird aufgerufen bei Flankenänderung an PA1 (Taster).
 */
void EXTI1_IRQHandler(void) {
    /* Sicherstellen, dass es wirklich ein EXTI1 Interrupt ist */
    if (EXTI->PR & EXTI_PR_PR1) {
        /* Zeitpunkt des Interrupts speichern */
        last_exti_time = systick_now_ms();
        
        /* Flag setzen für Hauptprogramm */
        exti_triggered = 1;
        
        /* Interrupt-Flag löschen (durch Schreiben 1) */
        EXTI->PR = EXTI_PR_PR1;
    }
}

/**
 * @brief Initialisiert alle GPIO-Pins für das Projekt
 */
static void gpio_init_all(void) {
    /* LED an PC13 (active-low) */
    gpio_init(GPIOC, 13, GPIO_MODE_OUTPUT_2MHZ, GPIO_CNF_OUTPUT_PUSHPULL);
    
    /* PA0: Rohsignal-Ausgang (für Messungen mit LA/Oszilloskop) */
    gpio_init(GPIOA, 0, GPIO_MODE_OUTPUT_10MHZ, GPIO_CNF_OUTPUT_PUSHPULL);
    
    /* PA2: Entprelltes Signal-Ausgang (nach Filter) */
    gpio_init(GPIOA, 2, GPIO_MODE_OUTPUT_10MHZ, GPIO_CNF_OUTPUT_PUSHPULL);
    
    /* PA3: Prellen-Indikator LED (blinkt bei Prellen-Ereignissen) */
    gpio_init(GPIOA, 3, GPIO_MODE_OUTPUT_10MHZ, GPIO_CNF_OUTPUT_PUSHPULL);
    
    /* PA1: Taster-Eingang mit internem Pull-up */
    gpio_init(GPIOA, 1, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL);
}

/**
 * @brief Setzt alle Ausgänge auf ihren Initialzustand
 */
static void gpio_set_initial_state(void) {
    /* PC13 HIGH = LED aus (active-low) */
    gpio_set(GPIOC, 13);
    
    /* PA0 LOW = Rohsignal startet mit LOW */
    gpio_clear(GPIOA, 0);
    
    /* PA2 LOW = Entprelltes Signal startet mit LOW */
    gpio_clear(GPIOA, 2);
    
    /* PA3 LOW = Prellen-Indikator aus */
    gpio_clear(GPIOA, 3);
}

/**
 * @brief Konfiguriert EXTI für Taster PA1
 */
static void exti_setup(void) {
    /* EXTI für PA1 mit beiden Flanken konfigurieren */
    exti_init((uint32_t)GPIOA, 1, EXTI_TRIGGER_BOTH);
    
    /* NVIC für EXTI1 Interrupt konfigurieren (mittlere Priorität) */
    nvic_enable_irq(EXTI1_IRQn, NVIC_PRIORITY_MEDIUM);
    
    /* EXTI-Interrupt aktivieren */
    exti_enable_interrupt();
    
    /* Globale Interrupts aktivieren */
    nvic_enable_global_interrupts();
}

/**
 * @brief Hauptfunktion
 *
 * Demonstrationsablauf:
 * 1. SysTick für 1ms Zeitbasis starten
 * 2. GPIO und EXTI initialisieren
 * 3. Entprellungslogik initialisieren
 * 4. Hauptschleife mit WFI (Wait For Interrupt):
 *    - Bei EXTI-Trigger: Rohsignal lesen und Entprellung aktualisieren
 *    - Ausgaben aktualisieren (LED, Debug-Pins)
 *    - Wieder in WFI gehen
 */
int main(void) {
    uint32_t last_update_time = 0U;
    uint16_t last_bounce_count = 0U;
    bool prellen_indicator_state = false;
    uint32_t prellen_blink_time = 0U;
    
    /* SysTick als 1ms Systemzeitbasis starten */
    systick_init_1ms();
    
    /* GPIO-Pins initialisieren */
    gpio_init_all();
    gpio_set_initial_state();
    
    /* EXTI für Taster PA1 konfigurieren */
    exti_setup();
    
    /* Entprellungslogik initialisieren */
    debounce_init();
    
    /* Hauptschleife */
    while(1) {
        uint32_t current_time = systick_now_ms();
        
        /* Prüfen, ob EXTI ausgelöst wurde */
        if (exti_triggered) {
            exti_triggered = 0;
            
            /* Rohsignal von PA1 lesen (jetzt stabil nach Interrupt) */
            uint8_t raw_state = gpio_read(GPIOA, 1);
            
            /* Rohsignal auf PA0 spiegeln (für Messungen mit LA/Oszilloskop) */
            if (raw_state == 0U) {
                gpio_set(GPIOA, 0);    /* Taster gedrückt → PA0 HIGH */
            } else {
                gpio_clear(GPIOA, 0);  /* Taster losgelassen → PA0 LOW */
            }
            
            /* Entprellungslogik aktualisieren (mit Zeitstempel vom Interrupt) */
            debounce_update(raw_state);
            last_update_time = current_time;
        }
        
        /* Alle 1ms prüfen, ob Entprellung aktualisiert werden muss */
        /* (für Timeouts der Zustandsmaschine) */
        if ((current_time - last_update_time) >= 1U) {
            /* Falls kein EXTI kam, trotzdem Rohsignal lesen für Timeout-Logik */
            uint8_t raw_state = gpio_read(GPIOA, 1);
            debounce_update(raw_state);
            last_update_time = current_time;
        }
        
        /* Entprelltes Signal auf PA2 ausgeben */
        bool debounced_state = debounce_get_state();
        if (debounced_state) {
            gpio_set(GPIOA, 2);    /* Entprellter Taster gedrückt → PA2 HIGH */
        } else {
            gpio_clear(GPIOA, 2);  /* Entprellter Taster losgelassen → PA2 LOW */
        }
        
        /* PC13 LED basierend auf entprelltem Signal steuern */
        if (debounced_state) {
            gpio_clear(GPIOC, 13); /* LED an (active-low) */
        } else {
            gpio_set(GPIOC, 13);   /* LED aus */
        }
        
        /* Prellen-Indikator (PA3) blinken lassen bei neuen Prellen-Ereignissen */
        uint16_t current_bounce_count = debounce_get_bounce_count();
        if (current_bounce_count != last_bounce_count) {
            /* Neues Prellen-Event erkannt → Indikator für 100ms blinken */
            prellen_indicator_state = true;
            prellen_blink_time = current_time;
            last_bounce_count = current_bounce_count;
        }
        
        /* Prellen-Indikator nach 100ms ausschalten */
        if (prellen_indicator_state && 
            (current_time - prellen_blink_time) >= 100U) {
            prellen_indicator_state = false;
        }
        
        /* Prellen-Indikator setzen/rücksetzen */
        if (prellen_indicator_state) {
            gpio_set(GPIOA, 3);    /* Prellen-Indikator an */
        } else {
            gpio_clear(GPIOA, 3);  /* Prellen-Indikator aus */
        }
        
        /* WFI (Wait For Interrupt) - CPU schlafen legen bis nächster Interrupt */
        /* Spart Energie im Vergleich zu Projekt 04 (Polling mit 100% CPU-Auslastung) */
        __WFI();
    }
    
    return 0;
}