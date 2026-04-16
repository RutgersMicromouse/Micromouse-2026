#include "timer.h"
#include "stm32g070xx.h"

void init_encoder_tim1(void)
{
    /* 1. Enable Clocks */
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;    // Enable GPIOA clock
    RCC->APBENR2 |= RCC_APBENR2_TIM1EN;   // Enable TIM1 clock

    /* 2. Configure PA8 and PA9 to Alternate Function 2 (TIM1_CH1 & CH2) */
    // Clear mode bits and set to Alternate Function (10)
    GPIOA->MODER &= ~(GPIO_MODER_MODE8 | GPIO_MODER_MODE9);
    GPIOA->MODER |= (GPIO_MODER_MODE8_1 | GPIO_MODER_MODE9_1);
    
    // Select AF2 for PA8 and PA9 in the Alternate Function High Register (AFRH)
	GPIOA->AFR[1] &= ~(GPIO_AFRH_AFSEL8 | GPIO_AFRH_AFSEL9);
	GPIOA->AFR[1] |= (2U << GPIO_AFRH_AFSEL8_Pos) | (2U << GPIO_AFRH_AFSEL9_Pos);

    /* 3. Configure TIM1 for Encoder Mode 3 */
    // Map IC1 to TI1 (CC1S=01) and IC2 to TI2 (CC2S=01)
    TIM1->CCMR1 = (1U << TIM_CCMR1_CC1S_Pos) | (1U << TIM_CCMR1_CC2S_Pos); 
    
    // Set Slave Mode Controller to Encoder Mode 3 (SMS=011)
    // This counts on both TI1 and TI2 edges for maximum resolution
    TIM1->SMCR = (TIM1->SMCR & ~TIM_SMCR_SMS_Msk) | (3U << TIM_SMCR_SMS_Pos);
    
    // Set Auto-Reload to maximum (16-bit)
    TIM1->ARR = 0xFFFF;

    /* 4. Reset counter and start the timer */
    TIM1->CNT = 0;
    TIM1->CR1 |= TIM_CR1_CEN;
}

void init_timer(void)
{
    init_encoder_tim1();
}

uint16_t read_encoder_position(void)
{
    // The hardware automatically updates this register based on rotation
    return TIM1->CNT; 
}