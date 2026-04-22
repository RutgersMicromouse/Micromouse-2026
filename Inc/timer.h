#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

/**
 * @brief Initialize TIM3 for a 1ms tick
 */
void timer_init(void);

/**
 * @brief Get the current tick count (ms)
 * @return uint32_t current ticks
 */
uint32_t timer_get_tick(void);

#endif
