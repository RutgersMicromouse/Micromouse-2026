#ifndef MOTOR_H_
#define MOTOR_H_

#include <stdint.h>
#include "stm32g070xx.h"

// Motor Control Modes
#define MOTOR_CW    1
#define MOTOR_CCW   2
#define MOTOR_STOP  3
#define MOTOR_BRAKE 4

/**
 * @brief Initialize the TB6612FNG motor driver.
 * Configures PA8 (TIM1_CH1) and PA9 (TIM1_CH2) for PWM,
 * PC6, PC7 for Motor A direction, and PD8, PD9 for Motor B direction.
 */
void motor_init(void);

/**
 * @brief Control Motor A.
 * @param power Power level (-800 to 800). Positive for forward (CW), negative for backward (CCW).
 */
void motor_a_drive(int16_t power);

/**
 * @brief Control Motor B.
 * @param power Power level (-800 to 800). Positive for forward (CW), negative for backward (CCW).
 */
void motor_b_drive(int16_t power);

#endif /* MOTOR_H_ */
