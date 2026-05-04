#include "motor.h"

#define GPIOAEN             (1U << 0)
#define GPIOCEN             (1U << 2)
#define GPIODEN             (1U << 3)
#define TIM1EN              (1U << 11)

// PWM Mode 1: OC1M = 110
#define PWM_MODE_1_CH1      (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2)
#define PWM_MODE_1_CH2      (TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2)

void motor_init(void) 
{
    // 1. Enable Peripheral Clocks
    RCC->IOPENR |= GPIOAEN | GPIOCEN | GPIODEN;
    RCC->APBENR2 |= TIM1EN;

    // 2. Configure Direction Pins (PC6, PC7, PD8, PD9 as outputs)
    // GPIOC: PC6 and PC7
    GPIOC->MODER &= ~((3U << (6 * 2)) | (3U << (7 * 2)));
    GPIOC->MODER |=  ((1U << (6 * 2)) | (1U << (7 * 2)));
    
    // GPIOD: PD8 and PD9
    GPIOD->MODER &= ~((3U << (8 * 2)) | (3U << (9 * 2)));
    GPIOD->MODER |=  ((1U << (8 * 2)) | (1U << (9 * 2)));

    // 3. Configure PWM Pins (PA8 and PA9 as Alternate Function 2)
    GPIOA->MODER &= ~((3U << (8 * 2)) | (3U << (9 * 2)));
    GPIOA->MODER |=  ((2U << (8 * 2)) | (2U << (9 * 2))); 
    
    // PA8 and PA9 are in AFR[1] (High register) for STM32G0
    GPIOA->AFR[1] &= ~((0xFU << ((8 - 8) * 4)) | (0xFU << ((9 - 8) * 4)));
    GPIOA->AFR[1] |=  ((2U << ((8 - 8) * 4))   | (2U << ((9 - 8) * 4)));

    // 4. Configure TIM1 for PWM Output
    // Assuming a 16 MHz System Clock (as seen in uart.c), 
    // to get 20kHz: 16,000,000 / 20,000 = 800
    
    TIM1->PSC = 0;              
    TIM1->ARR = 800 - 1;       

    // Set Channel 1 (PA8) and Channel 2 (PA9) to PWM Mode 1
    TIM1->CCMR1 &= ~(TIM_CCMR1_OC1M_Msk | TIM_CCMR1_OC2M_Msk); // Clear OC1M and OC2M
    TIM1->CCMR1 |= PWM_MODE_1_CH1 | TIM_CCMR1_OC1PE;
    TIM1->CCMR1 |= PWM_MODE_1_CH2 | TIM_CCMR1_OC2PE;

    // Enable Channel 1 and Channel 2 Outputs
    TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E;

    // Enable Main Output Enable (MOE) required for TIM1
    TIM1->BDTR |= TIM_BDTR_MOE;

    // Start TIM1 Counter
    TIM1->CR1 |= TIM_CR1_CEN;
}

void motor_a_drive(int16_t power) 
{
    if (power > 800) power = 800;
    if (power < -800) power = -800;
    
    if (power > 0) {
        // Forward (CW)
        TIM1->CCR1 = (uint16_t)power;
        // Set PC6 (AIN1) HIGH, Reset PC7 (AIN2) LOW
        GPIOC->BSRR = (1U << 6) | (1U << (7 + 16));
    } else if (power < 0) {
        // Backward (CCW)
        TIM1->CCR1 = (uint16_t)(-power);
        // Reset PC6 (AIN1) LOW, Set PC7 (AIN2) HIGH
        GPIOC->BSRR = (1U << (6 + 16)) | (1U << 7);
    } else {
        // Stop
        TIM1->CCR1 = 0;
        GPIOC->BSRR = (1U << (6 + 16)) | (1U << (7 + 16));
    }
}

void motor_b_drive(int16_t power) 
{
    if (power > 800) power = 800;
    if (power < -800) power = -800;
    
    if (power > 0) {
        // Forward (CW)
        TIM1->CCR2 = (uint16_t)power;
        // Set PD8 (BIN1) HIGH, Reset PD9 (BIN2) LOW
        GPIOD->BSRR = (1U << 8) | (1U << (9 + 16));
    } else if (power < 0) {
        // Backward (CCW)
        TIM1->CCR2 = (uint16_t)(-power);
        // Reset PD8 (BIN1) LOW, Set PD9 (BIN2) HIGH
        GPIOD->BSRR = (1U << (8 + 16)) | (1U << 9);
    } else {
        // Stop
        TIM1->CCR2 = 0;
        GPIOD->BSRR = (1U << (8 + 16)) | (1U << (9 + 16));
    }
}
