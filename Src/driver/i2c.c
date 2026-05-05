/**
 * @file i2c.c
 * @brief I2C1 driver implementation for STM32G070RB microcontroller.
 * This file contains functions for initializing the i2c1 peripheral,
 * reading and writing to i2c devices.
 */
#include "i2c.h"
#include "stm32g070xx.h"
#include <stdint.h>

#define I2C1EN (1U << 21) //I2C pins pb6 and pb7 connect to I2C1
#define I2C2EN (1U << 22)
#define GPIOBEN (1U << 1)
#define I2C1_SCL (6U << 24)
#define I2C1_SDA (6U << 28)
#define OTYPERVAL (3U << 6)
#define I2CSPEED (0xAU << 12)
#define I2CPRESC (1U << 28)
#define I2CSCLL (0x9U)
#define I2CSCLH (0x3U << 8)
#define I2CSDADEL (0x2U << 16)
#define I2CSCLDEL (0x3U << 20)

#define I2C_TIMEOUT 10000

/* I2C Result String Array */
const char* myerrarray[3] = {
		"i2c SUCCESS",
		"i2c ERROR - Timed Out",
		"i2c ERROR - MISC ERROR"
};

/**
 * @brief Initializes the I2C1 peripheral and configures GPIO pins PB6 and PB7 for I2C functionality.
 * This function enables the necessary clocks, sets up the GPIO pins for alternate function,
 * configures the I2C timing parameters, and enables the I2C1 peripheral.
 */
void init_i2c(void)
{
	/* enable i2c and __gpiob__ peripheral circuit */
	RCC->APBENR1 |= I2C1EN;
	RCC->IOPENR |= GPIOBEN;

	/* configure PB6 and PB7 to use alternate functions.*/

	/* clear PB6 and PB7 function */
	GPIOB->MODER &= ~((3U << 12) | (3U << 14));
    GPIOB->MODER |= (2U << 12) | (2U << 14);
	
	/* configure PB6 and PB7 as alternate function. */
	GPIOB->AFR[0] &= ~((0xFU << 24) | (0xFU << 28));
	GPIOB->AFR[0] |= (I2C1_SCL | I2C1_SDA);

	/* enable output open-drain for PB6 and PB7 */
	GPIOB->OTYPER |= OTYPERVAL;

	/* configure GPIO port output speed */

	/* clear port output speed for PB6 and PB7 */
	GPIOB->OSPEEDR &= ~(0xFU << 12);

	/* configure PB6 and PB7 to use high speed*/
	GPIOB->OSPEEDR |= I2CSPEED;

	/* enable internal pull on GPIOB_PUPDR */

	/* clear bits for PB6 and PB7 */
	GPIOB->PUPDR &= ~(0xFU << 12);

	/* set pull ups on PB6 and PB7 */
	GPIOB->PUPDR |= ((1U << 12) | (1U << 14));

	/* configure I2C Timing */

	/* ensure I2C is disabled */
	I2C1->CR1 &= ~(1U << 0);

	/* configure timing for i2c1 */

	/* clear timer setting register */
	I2C1->TIMINGR = 0;

	/* set PRESC */
	I2C1->TIMINGR |= I2CPRESC;
	/* Set SCLL AND SCLH*/
	I2C1->TIMINGR |= (I2CSCLL | I2CSCLH);
	/* set SDADEL and SCLDEL */
	I2C1->TIMINGR |= (I2CSDADEL | I2CSCLDEL);

	/* Enable I2C1 peripheral */
	I2C1->CR1 |= (1U << 0);
}

/**
 * @brief Reads a byte from a specified register of an I2C slave device.
 * @param slave_address The 7-bit address of the I2C slave device.
 * @param reg_address The register address to read from.
 * @param val Pointer to store the read byte.
 * @return i2c_result indicating success or type of error.
 */
i2c_result i2cread(uint8_t slave_address, uint8_t reg_address, uint8_t* val)
{
	/* Configure for write operation - send register address */
	I2C1->CR2 = 0; // Clear CR2
	I2C1->CR2 = (slave_address << 1) | (1U << 16) | I2C_CR2_START;
	uint32_t timeout = I2C_TIMEOUT;

	/* Wait for TXIS (Transmit register empty) */
	while (!(I2C1->ISR & I2C_ISR_TXIS)) {
		timeout--;
		if (timeout == 0) return i2c_timeout;
	}

	I2C1->TXDR = reg_address; // Send Register Address

	/* Wait for Transfer Complete (TC) */
	timeout = I2C_TIMEOUT;
	while (!(I2C1->ISR & I2C_ISR_TC)) {
		timeout--;
		if (timeout == 0) return i2c_timeout;
	}

	/* Configure for read operation */
	I2C1->CR2 = (slave_address << 1) | (1U << 16) | I2C_CR2_RD_WRN | I2C_CR2_AUTOEND | I2C_CR2_START;

	/* Wait for data ready */
	timeout = I2C_TIMEOUT;
	while (!(I2C1->ISR & I2C_ISR_RXNE)) {
		timeout--;
		if (timeout == 0) return i2c_timeout;
	}
	uint8_t data = (uint8_t)I2C1->RXDR; // Read Data

	/* Wait for Stop */
	timeout = I2C_TIMEOUT;
	while (!(I2C1->ISR & I2C_ISR_STOPF)) {
		timeout--;
		if (timeout == 0) return i2c_timeout;
	}
	I2C1->ICR = I2C_ICR_STOPCF; // Clear Stop Flag

	*val = data;
	return i2c_success;
}

/**
 * @brief Writes a byte to a specified register of an I2C slave device.
 * @param slave_addr The 7-bit address of the I2C slave device.
 * @param reg_addr The register address to write to.
 * @param data The byte to write.
 * @return i2c_result indicating success or type of error.
 */
i2c_result i2cwrite(uint8_t slave_addr, uint8_t reg_addr, uint8_t data)
{
    // 1. Configure CR2: Write 2 bytes (Reg Addr + Data), Autoend, Start
    I2C1->CR2 = (slave_addr << 1) | (2 << 16) | I2C_CR2_AUTOEND | I2C_CR2_START;
    // 2. Send Register Address
    uint32_t timeout = I2C_TIMEOUT;
    while (!(I2C1->ISR & I2C_ISR_TXIS)) {
		timeout--;
		if (timeout == 0) return i2c_timeout;
	}
    I2C1->TXDR = reg_addr;

    // 3. Send Data
    timeout = I2C_TIMEOUT;
    while (!(I2C1->ISR & I2C_ISR_TXIS)) {
		timeout--;
		if (timeout == 0) return i2c_timeout;
	}
    I2C1->TXDR = data;

    // 4. Wait for Stop
    timeout = I2C_TIMEOUT;
    while (!(I2C1->ISR & I2C_ISR_STOPF)) {
		timeout--;
		if (timeout == 0) return i2c_timeout;
	}
    I2C1->ICR = I2C_ICR_STOPCF; // Clear Stop Flag
    return i2c_success;
}
