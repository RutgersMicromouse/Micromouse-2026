#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>

/**
 * @brief Initializes the left and right sensors on PB13 and PC3 using EXTI.
 * PB13: Left Sensor
 * PC3: Right Sensor
 */
void init_sensors(void);

/**
 * @brief Returns the distance measured by the left sensor.
 * @return distance in the units defined by the formula d = 2 * (t - 1000)
 */
uint32_t get_left_distance(void);

/**
 * @brief Returns the distance measured by the right sensor.
 * @return distance in the units defined by the formula d = 2 * (t - 1000)
 */
uint32_t get_right_distance(void);

#endif // SENSOR_H
