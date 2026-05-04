/** 
* @file i2c.h
* @brief I2C2 driver header for STM32G070RB microcontroller.
* This file contains the declarations for initializing the I2C2 peripheral,
* reading from, and writing to I2C slave devices.
*/
#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>

typedef enum {
	i2c_success,
	i2c_timeout,
	i2c_error,
} i2c_result;

extern const char* myerrarray[3];  // Declaration only

#define i2ctostr(index) ((index) < 3 ? myerrarray[(index)] : "i2c ERROR - Invalid Index")

void init_i2c();
i2c_result i2c_is_device_ready(uint8_t slave_addr);
void i2c_scan();
i2c_result i2cread(uint8_t slave_address, uint8_t reg_address, uint8_t* val);
i2c_result i2cread16(uint8_t slave_addr, uint16_t reg_addr, uint8_t* val);
i2c_result i2cwrite(uint8_t slave_addr, uint8_t reg_addr, uint8_t data);
i2c_result i2cwrite16(uint8_t slave_addr, uint16_t reg_addr, uint8_t data);

#include "stm32g070xx.h"
uint16_t I2C_Read16(I2C_TypeDef *I2Cx, uint8_t dev_addr, uint16_t reg_addr);

#endif /* I2C_H_ */
