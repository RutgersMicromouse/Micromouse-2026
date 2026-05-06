/**
 * @file bno055_sensor.c
 * @brief Application-level driver to get angle information from BNO055.
 */

#include "i2c.h"
#include <stdint.h>
#include "bno055.h"
#include <stdio.h>

#include "uart.h"

#define BNO055_OPR_MODE_REG    0x3D
#define BNO055_UNIT_SEL_REG    0x3B
#define BNO055_MODE_NDOF       0x0C // Nine Degrees of Freedom
#define BNO055_MODE_IMU        0x08 // Relative orientation (useful if no mag calibration)

 /* * BNO055 I2C Address [3]
 * Default is 0x28 when COM3 is LOW, or 0x29 when COM3 is HIGH.
 */
#define BNO055_I2C_ADDR    0x28 

/* 
 * Euler Angle Registers (Page 0) [4-6]
 * Each angle consists of a signed 16-bit integer (2 bytes).
 */
#define EUL_HEADING_LSB    0x1A
#define EUL_ROLL_LSB       0x1C
#define EUL_PITCH_LSB      0x1E

/* 
 * Scale Factor for Degrees [7, 8]
 * 1 Degree = 16 LSB
 */
#define EULER_SCALE_DEG    16.0f


/**
 * @brief Reads Heading, Roll, and Pitch from the sensor.
 * @note This data is only available in fusion operation modes [8, 9].
 * @param angles Pointer to bno055_euler_t to store converted degree values.
 * @return i2c_result indicating success or a specific timeout/error [1].
 */

 i2c_result bno055_init(void) {
    i2c_result res;
    
    // Switch to NDOF mode to enable fusion (Euler angles)
    uint8_t mode = BNO055_MODE_NDOF;
    res = i2cwrite(BNO055_I2C_ADDR, BNO055_OPR_MODE_REG, mode);
    
    // Note: The BNO055 usually needs ~20ms to switch modes.
    // If you have a delay function, call it here.
    
    return res;
}
i2c_result bno055_get_angles(bno055_euler_t *angles) {
    uint8_t lsb, msb;
    int16_t raw;
    i2c_result res;

    /* 1. Get Heading (Yaw) [10, 11] */
    res = i2cread(BNO055_I2C_ADDR, EUL_HEADING_LSB, &lsb);
    if (res != i2c_success) return res;
    res = i2cread(BNO055_I2C_ADDR, EUL_HEADING_LSB + 1, &msb);
    if (res != i2c_success) return res;
    raw = (int16_t)((msb << 8) | lsb);
    angles->heading = (float)raw / EULER_SCALE_DEG;

    /* 2. Get Roll [12, 13] */
    res = i2cread(BNO055_I2C_ADDR, EUL_ROLL_LSB, &lsb);
    if (res != i2c_success) return res;
    res = i2cread(BNO055_I2C_ADDR, EUL_ROLL_LSB + 1, &msb);
    if (res != i2c_success) return res;
    raw = (int16_t)((msb << 8) | lsb);
    angles->roll = (float)raw / EULER_SCALE_DEG;

    /* 3. Get Pitch [14, 15] */
    res = i2cread(BNO055_I2C_ADDR, EUL_PITCH_LSB, &lsb);
    if (res != i2c_success) return res;
    res = i2cread(BNO055_I2C_ADDR, EUL_PITCH_LSB + 1, &msb);
    if (res != i2c_success) return res;
    raw = (int16_t)((msb << 8) | lsb);
    angles->pitch = (float)raw / EULER_SCALE_DEG;

    return i2c_success;
}

