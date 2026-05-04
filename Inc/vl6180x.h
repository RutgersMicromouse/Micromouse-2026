#ifndef VL6180X_H
#define VL6180X_H

#include <stdint.h>
#include "i2c.h"

#define VL6180X_ADDR 0x29
#define VL6180X_ADDR_ALT 0x2B

// VL6180X Registers
#define SYSRANGE__START                0x018
#define SYSTEM__INTERRUPT_CLEAR        0x015
#define SYSTEM__FRESH_OUT_OF_RESET     0x016
#define RESULT__INTERRUPT_STATUS_GPIO  0x04F
#define RESULT__RANGE_VAL              0x062

/**
 * @brief Initialize the VL6180X sensor.
 */
i2c_result vl6180x_init(void);

/**
 * @brief Read distance from VL6180X using polling mode.
 * @param distance_mm Pointer to store the result.
 * @return i2c_result indicating success or failure.
 */
i2c_result vl6180x_read_distance(uint8_t *distance_mm);

#endif // VL6180X_H
