#include "sensor.h"
#include "stm32g070xx.h"

// Global variables to store pulse start times and distances
static volatile uint32_t left_start_time = 0;
static volatile uint32_t right_start_time = 0;
static volatile uint32_t left_distance = 0;
static volatile uint32_t right_distance = 0;

void init_sensors(void) {
    // 1. Enable clocks for GPIOB, GPIOC, and TIM6
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN | RCC_IOPENR_GPIOCEN;
    RCC->APBENR1 |= RCC_APBENR1_TIM6EN;

    // 2. Configure PB13 and PC3 as Input
    // PB13
    GPIOB->MODER &= ~(GPIO_MODER_MODE13_Msk);
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD13_Msk); // Floating
    
    // PC3
    GPIOC->MODER &= ~(GPIO_MODER_MODE3_Msk);
    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD3_Msk); // Floating

    // 3. Configure EXTI Routing
    // PB13 to EXTI13 (EXTICR4, index 3, bits 8-10, value 1 for Port B)
    EXTI->EXTICR[3] &= ~(EXTI_EXTICR4_EXTI13_Msk);
    EXTI->EXTICR[3] |= (1U << EXTI_EXTICR4_EXTI13_Pos);

    // PC3 to EXTI3 (EXTICR1, index 0, bits 24-26, value 2 for Port C)
    EXTI->EXTICR[0] &= ~(EXTI_EXTICR1_EXTI3_Msk);
    EXTI->EXTICR[0] |= (2U << EXTI_EXTICR1_EXTI3_Pos);

    // 4. Configure EXTI Triggers (Both Edges)
    EXTI->RTSR1 |= (EXTI_RTSR1_RT13 | EXTI_RTSR1_RT3);
    EXTI->FTSR1 |= (EXTI_FTSR1_FT13 | EXTI_FTSR1_FT3);

    // 5. Unmask EXTI Interrupts
    EXTI->IMR1 |= (EXTI_IMR1_IM13 | EXTI_IMR1_IM3);

    // 6. Configure TIM6 for 1us ticks (assuming 16MHz HSI)
    TIM6->PSC = 16 - 1; // 16MHz / 16 = 1MHz
    TIM6->ARR = 0xFFFF;
    TIM6->EGR |= TIM_EGR_UG; // Generate an update event to reload the prescaler
    TIM6->CR1 |= TIM_CR1_CEN;

    // 7. Enable NVIC Interrupts
    NVIC_SetPriority(EXTI2_3_IRQn, 0);
    NVIC_EnableIRQ(EXTI2_3_IRQn);
    NVIC_SetPriority(EXTI4_15_IRQn, 0);
    NVIC_EnableIRQ(EXTI4_15_IRQn);
}

uint32_t get_left_distance(void) {
    return left_distance;
}

uint32_t get_right_distance(void) {
    return right_distance;
}

// ISR for PC3 (Right Sensor)
void EXTI2_3_IRQHandler(void) {
    if (EXTI->RPR1 & EXTI_RPR1_RPIF3 || EXTI->FPR1 & EXTI_FPR1_FPIF3) {
        if (GPIOC->IDR & GPIO_IDR_ID3) {
            // Rising edge
            right_start_time = TIM6->CNT;
        } else {
            // Falling edge
            uint32_t end_time = TIM6->CNT;
            uint32_t duration;
            if (end_time >= right_start_time) {
                duration = end_time - right_start_time;
            } else {
                duration = (0xFFFF - right_start_time) + end_time + 1;
            }
            // Formula: d = 2 * (t - 1000)
            if (duration > 1000) {
                right_distance = 2 * (duration - 1000);
            } else {
                right_distance = 0;
            }
        }
        // Clear pending flags
        EXTI->RPR1 = EXTI_RPR1_RPIF3;
        EXTI->FPR1 = EXTI_FPR1_FPIF3;
    }
}

// ISR for PB13 (Left Sensor)
void EXTI4_15_IRQHandler(void) {
    if (EXTI->RPR1 & EXTI_RPR1_RPIF13 || EXTI->FPR1 & EXTI_FPR1_FPIF13) {
        if (GPIOB->IDR & GPIO_IDR_ID13) {
            // Rising edge
            left_start_time = TIM6->CNT;
        } else {
            // Falling edge
            uint32_t end_time = TIM6->CNT;
            uint32_t duration;
            if (end_time >= left_start_time) {
                duration = end_time - left_start_time;
            } else {
                duration = (0xFFFF - left_start_time) + end_time + 1;
            }
            // Formula: d = 2 * (t - 1000)
            if (duration > 1000) {
                left_distance = 2 * (duration - 1000);
            } else {
                left_distance = 0;
            }
        }
        // Clear pending flags
        EXTI->RPR1 = EXTI_RPR1_RPIF13;
        EXTI->FPR1 = EXTI_FPR1_FPIF13;
    }
}
