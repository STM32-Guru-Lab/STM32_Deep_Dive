/**
 * @file systick.h
 * @brief Systemzeitbasis auf Basis des SysTick-Timers.
 */

#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

void systick_init_1ms(void);
uint32_t systick_now_ms(void);
void systick_delay_ms(uint32_t delay_ms);
uint8_t systick_period_elapsed(uint32_t *last_ms, uint32_t period_ms);

#endif /* SYSTICK_H */
