#include "timer.h"
#include "stm32g070xx.h"

static volatile uint32_t ms_ticks = 0;

void timer_init(void)
{
    /* Enable TIM3 clock */
    RCC->APBENR1 |= RCC_APBENR1_TIM3EN;
    (void)RCC->APBENR1; // Dummy read

    /*
     * SystemCoreClock is 16MHz by default.
     * To get 1ms tick:
     * PSC = 1600 - 1  => timer frequency = 16MHz / 1600 = 10kHz
     * ARR = 10 - 1    => interrupt frequency = 10kHz / 10 = 1kHz (1ms)
     */
    TIM3->PSC = 1600 - 1;
    TIM3->ARR = 10 - 1;

    /* Enable Update Interrupt */
    TIM3->DIER |= TIM_DIER_UIE;

    /* Enable TIM3 */
    TIM3->CR1 |= TIM_CR1_CEN;

    /* Enable TIM3 IRQ in NVIC */
    NVIC_SetPriority(TIM3_IRQn, 3);
    NVIC_EnableIRQ(TIM3_IRQn);
}

uint32_t timer_get_tick(void)
{
    return ms_ticks;
}

void TIM3_IRQHandler(void)
{
    if (TIM3->SR & TIM_SR_UIF) {
        TIM3->SR &= ~TIM_SR_UIF; // Clear update interrupt flag
        ms_ticks++;
    }
}
