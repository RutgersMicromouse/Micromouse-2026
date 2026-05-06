#include "delay.h"
#include "timer.h"

void delay_ms(uint32_t ms)
{
    uint32_t start = timer_get_tick();
    while ((timer_get_tick() - start) < ms);
}
