/**
 * @file i2c.c
 * @brief I2C2 driver implementation for STM32G070RB microcontroller.
 * This file contains functions for initializing the i2c2 peripheral,
 * reading and writing to i2c devices.
 */
#include "i2c.h"
#include "stm32g070xx.h"
#include <stdint.h>
#include <stdio.h>

#define I2C2EN (1U << 22)
#define GPIOBEN (1U << 1)
#define I2C2_SCL (6U << 20)
#define I2C2_SDA (6U << 24)
#define OTYPERVAL (3U << 13)
#define I2CSPEED (0xAU << 26)
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
 * @brief Initializes the I2C2 peripheral and configures GPIO pins PB13 and PB14 for I2C functionality.
 * This function enables the necessary clocks, sets up the GPIO pins for alternate function,
 * configures the I2C timing parameters, and enables the I2C2 peripheral.
 */
void init_i2c(void)
{
	/* enable i2c and __gpiob__ peripheral circuit */
	RCC->APBENR1 |= I2C2EN;
	RCC->IOPENR |= GPIOBEN;

	/* configure PB13 and PB14 to use alternate functions.*/

	/* clear PB13 and PB14 function */
	GPIOB->MODER &= ~((1U << 27) | (1U << 26));
	GPIOB->MODER &= ~((1U << 29) | (1U << 28));

	/* configure PB13 and PB14 as alternate function. */
	GPIOB->MODER |= (1U << 27); //configure PB13 as alternate function
	GPIOB->MODER |= (1U << 29);

	/* configure alternate function for PB13 and PB14*/

	/* clear current alternate function for PB13 and PB14*/
	GPIOB->AFR[1] &= ~((0xFU << 20) | (0xFU << 24));

	/* configure PB13 and PB14 to use AF6 */
	GPIOB->AFR[1] |= (I2C2_SCL | I2C2_SDA);

	/* enable output open-drain for PB13 and PB14 */
	GPIOB->OTYPER |= OTYPERVAL;

	/* configure GPIO port output speed */

	/* clear port output speed for PB13 and PB14 */
	GPIOB->OSPEEDR &= ~(0xFU << 26);

	/* configure PB13 and PB14 to use high speed*/
	GPIOB->OSPEEDR |= I2CSPEED;

	/* enable internal pull on GPIOB_PUPDR */

	/* clear bits for PB13 and PB14 */
	GPIOB->PUPDR &= ~(0xFU << 26);

	/* set pull ups on PB13 and PB14 */
	GPIOB->PUPDR |= ((1U << 26) | (1U << 28));

	/* configure I2C Timing */

	/* ensure I2C is disabled */
	I2C2->CR1 &= ~(1U << 0);

	/* configure timing for i2c2 */

	/* clear timer setting register */
	I2C2->TIMINGR = 0;

	/* set PRESC */
	I2C2->TIMINGR |= I2CPRESC;
	/* Set SCLL AND SCLH*/
	I2C2->TIMINGR |= (I2CSCLL | I2CSCLH);
	/* set SDADEL and SCLDEL */
	I2C2->TIMINGR |= (I2CSDADEL | I2CSCLDEL);

	/* Enable I2C2 peripheral */
	I2C2->CR1 |= (1U << 0);
}

/**
 * @brief Checks if a device at the specified address is ready.
 * @param slave_addr The 7-bit address of the I2C slave device.
 * @return i2c_result indicating success if device responded, error otherwise.
 */
i2c_result i2c_is_device_ready(uint8_t slave_addr)
{
	uint32_t timeout = I2C_TIMEOUT;

	/* Configure for 0-byte write operation to check for ACK */
	I2C2->CR2 = (slave_addr << 1) | (0 << 16) | I2C_CR2_AUTOEND | I2C_CR2_START;

	/* Wait for STOPF (device ACKed and transaction finished) or NACKF */
	while (!(I2C2->ISR & I2C_ISR_STOPF)) {
		if (I2C2->ISR & I2C_ISR_NACKF) {
			I2C2->ICR = I2C_ICR_NACKCF; // Clear NACK flag
			/* Wait for STOPF after NACK */
			while (!(I2C2->ISR & I2C_ISR_STOPF));
			I2C2->ICR = I2C_ICR_STOPCF; // Clear STOP flag
			return i2c_error;
		}
		if (--timeout == 0) return i2c_timeout;
	}

	I2C2->ICR = I2C_ICR_STOPCF; // Clear STOP flag
	return i2c_success;
}

/**
 * @brief Scans the I2C bus for devices and prints found addresses.
 */
void i2c_scan()
{
	printf("Scanning I2C bus...\n\r");
	int count = 0;
	for (uint8_t addr = 1; addr < 128; addr++) {
		if (i2c_is_device_ready(addr) == i2c_success) {
			printf("Found device at 0x%02X\n\r", addr);
			count++;
		}
	}
	if (count == 0) {
		printf("No I2C devices found.\n\r");
	} else {
		printf("Scan complete. %d devices found.\n\r", count);
	}
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
	I2C2->CR2 = 0; // Clear CR2
	I2C2->CR2 = (slave_address << 1) | (1U << 16) | I2C_CR2_START;
	uint32_t timeout = I2C_TIMEOUT;

	/* Wait for TXIS (Transmit register empty) */
	while (!(I2C2->ISR & I2C_ISR_TXIS)) {
		if (I2C2->ISR & I2C_ISR_NACKF) {
			I2C2->ICR = I2C_ICR_NACKCF;
			return i2c_error;
		}
		if (--timeout == 0) return i2c_timeout;
	}

	I2C2->TXDR = reg_address; // Send Register Address

	/* Wait for Transfer Complete (TC) */
	timeout = I2C_TIMEOUT;
	while (!(I2C2->ISR & I2C_ISR_TC)) {
		if (I2C2->ISR & I2C_ISR_NACKF) {
			I2C2->ICR = I2C_ICR_NACKCF;
			return i2c_error;
		}
		if (--timeout == 0) return i2c_timeout;
	}

	/* Configure for read operation */
	I2C2->CR2 = (slave_address << 1) | (1U << 16) | I2C_CR2_RD_WRN | I2C_CR2_AUTOEND | I2C_CR2_START;

	/* Wait for data ready */
	timeout = I2C_TIMEOUT;
	while (!(I2C2->ISR & I2C_ISR_RXNE)) {
		if (I2C2->ISR & I2C_ISR_NACKF) {
			I2C2->ICR = I2C_ICR_NACKCF;
			return i2c_error;
		}
		if (--timeout == 0) return i2c_timeout;
	}
	uint8_t data = (uint8_t)I2C2->RXDR; // Read Data

	/* Wait for Stop */
	timeout = I2C_TIMEOUT;
	while (!(I2C2->ISR & I2C_ISR_STOPF)) {
		if (--timeout == 0) return i2c_timeout;
	}
	I2C2->ICR = I2C_ICR_STOPCF; // Clear Stop Flag

	*val = data;
	return i2c_success;
}

/**
 * @brief Reads a byte from a specified 16-bit register of an I2C slave device.
 * @param slave_addr The 7-bit address of the I2C slave device.
 * @param reg_addr The 16-bit register address to read from.
 * @param val Pointer to store the read byte.
 * @return i2c_result indicating success or type of error.
 */
i2c_result i2cread16(uint8_t slave_addr, uint16_t reg_addr, uint8_t* val)
{
	/* Configure for write operation - send 2-byte register address */
	I2C2->CR2 = (slave_addr << 1) | (2 << 16) | I2C_CR2_START;
	uint32_t timeout = I2C_TIMEOUT;

	/* Wait for TXIS and write MSB */
	while (!(I2C2->ISR & I2C_ISR_TXIS)) {
		if (I2C2->ISR & I2C_ISR_NACKF) {
			I2C2->ICR = I2C_ICR_NACKCF;
			return i2c_error;
		}
		if (--timeout == 0) return i2c_timeout;
	}
	I2C2->TXDR = (uint8_t)(reg_addr >> 8);

	/* Wait for TXIS and write LSB */
	timeout = I2C_TIMEOUT;
	while (!(I2C2->ISR & I2C_ISR_TXIS)) {
		if (I2C2->ISR & I2C_ISR_NACKF) {
			I2C2->ICR = I2C_ICR_NACKCF;
			return i2c_error;
		}
		if (--timeout == 0) return i2c_timeout;
	}
	I2C2->TXDR = (uint8_t)(reg_addr & 0xFF);

	/* Wait for Transfer Complete (TC) */
	timeout = I2C_TIMEOUT;
	while (!(I2C2->ISR & I2C_ISR_TC)) {
		if (I2C2->ISR & I2C_ISR_NACKF) {
			I2C2->ICR = I2C_ICR_NACKCF;
			return i2c_error;
		}
		if (--timeout == 0) return i2c_timeout;
	}

	/* Configure for read operation - 1 byte */
	I2C2->CR2 = (slave_addr << 1) | (1 << 16) | I2C_CR2_RD_WRN | I2C_CR2_AUTOEND | I2C_CR2_START;

	/* Wait for data ready */
	timeout = I2C_TIMEOUT;
	while (!(I2C2->ISR & I2C_ISR_RXNE)) {
		if (I2C2->ISR & I2C_ISR_NACKF) {
			I2C2->ICR = I2C_ICR_NACKCF;
			return i2c_error;
		}
		if (--timeout == 0) return i2c_timeout;
	}
	*val = (uint8_t)I2C2->RXDR;

	/* Wait for Stop */
	timeout = I2C_TIMEOUT;
	while (!(I2C2->ISR & I2C_ISR_STOPF)) {
		if (--timeout == 0) return i2c_timeout;
	}
	I2C2->ICR = I2C_ICR_STOPCF;
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
    I2C2->CR2 = (slave_addr << 1) | (2 << 16) | I2C_CR2_AUTOEND | I2C_CR2_START;
    // 2. Send Register Address
    uint32_t timeout = I2C_TIMEOUT;
    while (!(I2C2->ISR & I2C_ISR_TXIS)) {
		if (I2C2->ISR & I2C_ISR_NACKF) {
			I2C2->ICR = I2C_ICR_NACKCF;
			return i2c_error;
		}
		if (--timeout == 0) return i2c_timeout;
	}
    I2C2->TXDR = reg_addr;

    // 3. Send Data
    timeout = I2C_TIMEOUT;
    while (!(I2C2->ISR & I2C_ISR_TXIS)) {
		if (I2C2->ISR & I2C_ISR_NACKF) {
			I2C2->ICR = I2C_ICR_NACKCF;
			return i2c_error;
		}
		if (--timeout == 0) return i2c_timeout;
	}
    I2C2->TXDR = data;

    // 4. Wait for Stop
    timeout = I2C_TIMEOUT;
    while (!(I2C2->ISR & I2C_ISR_STOPF)) {
		if (--timeout == 0) return i2c_timeout;
	}
    I2C2->ICR = I2C_ICR_STOPCF; // Clear Stop Flag
    return i2c_success;
}

/**
 * @brief Writes a byte to a specified 16-bit register of an I2C slave device.
 * @param slave_addr The 7-bit address of the I2C slave device.
 * @param reg_addr The 16-bit register address to write to.
 * @param data The byte to write.
 * @return i2c_result indicating success or type of error.
 */
i2c_result i2cwrite16(uint8_t slave_addr, uint16_t reg_addr, uint8_t data)
{
    // Configure CR2: Write 3 bytes (2 bytes for reg_addr + 1 byte for data)
    I2C2->CR2 = (slave_addr << 1) | (3 << 16) | I2C_CR2_AUTOEND | I2C_CR2_START;
    
    uint32_t timeout = I2C_TIMEOUT;
    // Send MSB of reg_addr
    while (!(I2C2->ISR & I2C_ISR_TXIS)) {
		if (I2C2->ISR & I2C_ISR_NACKF) {
			I2C2->ICR = I2C_ICR_NACKCF;
			return i2c_error;
		}
		if (--timeout == 0) return i2c_timeout;
	}
    I2C2->TXDR = (uint8_t)(reg_addr >> 8);

    // Send LSB of reg_addr
    timeout = I2C_TIMEOUT;
    while (!(I2C2->ISR & I2C_ISR_TXIS)) {
		if (I2C2->ISR & I2C_ISR_NACKF) {
			I2C2->ICR = I2C_ICR_NACKCF;
			return i2c_error;
		}
		if (--timeout == 0) return i2c_timeout;
	}
    I2C2->TXDR = (uint8_t)(reg_addr & 0xFF);

    // Send Data
    timeout = I2C_TIMEOUT;
    while (!(I2C2->ISR & I2C_ISR_TXIS)) {
		if (I2C2->ISR & I2C_ISR_NACKF) {
			I2C2->ICR = I2C_ICR_NACKCF;
			return i2c_error;
		}
		if (--timeout == 0) return i2c_timeout;
	}
    I2C2->TXDR = data;

    // Wait for Stop
    timeout = I2C_TIMEOUT;
    while (!(I2C2->ISR & I2C_ISR_STOPF)) {
		if (--timeout == 0) return i2c_timeout;
	}
    I2C2->ICR = I2C_ICR_STOPCF;
    return i2c_success;
}

/**
 * @brief Reads 2 bytes from a 16-bit register address using I2Cx.
 * @param I2Cx Pointer to I2C peripheral (e.g., I2C1, I2C2).
 * @param dev_addr The 7-bit slave address.
 * @param reg_addr The 16-bit register address.
 * @return The 16-bit value read (MSB first).
 */
uint16_t I2C_Read16(I2C_TypeDef *I2Cx, uint8_t dev_addr, uint16_t reg_addr)
{
    uint8_t msb, lsb;

    // =========================================================
    // PHASE 1: Write the 16-bit register address
    // =========================================================
    
    // Configure CR2: Slave Address, NBYTES = 2, Write (RD_WRN = 0), START = 1, AUTOEND = 0
    I2Cx->CR2 = (dev_addr << 1) | (2 << I2C_CR2_NBYTES_Pos) | I2C_CR2_START;

    // Wait for TXIS (Transmit Interrupt Status) and write the MSB of the register address
    while (!(I2Cx->ISR & I2C_ISR_TXIS)) {
        if (I2Cx->ISR & I2C_ISR_NACKF) {
            I2Cx->ICR = I2C_ICR_NACKCF;
            return 0xFFFF;
        }
    }
    I2Cx->TXDR = (uint8_t)(reg_addr >> 8);

    // Wait for TXIS again and write the LSB of the register address
    while (!(I2Cx->ISR & I2C_ISR_TXIS)) {
        if (I2Cx->ISR & I2C_ISR_NACKF) {
            I2Cx->ICR = I2C_ICR_NACKCF;
            return 0xFFFF;
        }
    }
    I2Cx->TXDR = (uint8_t)(reg_addr & 0xFF);

    // Wait for Transfer Complete (TC) flag
    while (!(I2Cx->ISR & I2C_ISR_TC)) {
        if (I2Cx->ISR & I2C_ISR_NACKF) {
            I2Cx->ICR = I2C_ICR_NACKCF;
            return 0xFFFF;
        }
    }

    // =========================================================
    // PHASE 2: Read the 2-byte data payload
    // =========================================================
    
    // Configure CR2: Slave Address, NBYTES = 2, Read (RD_WRN = 1), START = 1, AUTOEND = 1
    I2Cx->CR2 = (dev_addr << 1) | (2 << I2C_CR2_NBYTES_Pos) | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_AUTOEND;

    // Wait until Receive Data Register Not Empty (RXNE) flag goes high and read MSB
    while (!(I2Cx->ISR & I2C_ISR_RXNE)) {
        if (I2Cx->ISR & I2C_ISR_NACKF) {
            I2Cx->ICR = I2C_ICR_NACKCF;
            return 0xFFFF;
        }
    }
    msb = I2Cx->RXDR;

    // Wait for RXNE again and read LSB
    while (!(I2Cx->ISR & I2C_ISR_RXNE)) {
        if (I2Cx->ISR & I2C_ISR_NACKF) {
            I2Cx->ICR = I2C_ICR_NACKCF;
            return 0xFFFF;
        }
    }
    lsb = I2Cx->RXDR;

    // Wait for the STOP flag (STOPF) to confirm the transaction has finished completely
    while (!(I2Cx->ISR & I2C_ISR_STOPF)) {}

    // Clear the STOP flag by writing to the Interrupt Clear Register (ICR)
    I2Cx->ICR = I2C_ICR_STOPCF;

    // Combine the two 8-bit bytes into a single 16-bit value (MSB first)
    return ((uint16_t)msb << 8) | lsb;
}
