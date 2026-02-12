/**
 * This is supposed to show how to configure GPIO
 * and introduce the stm32g070xx.h header, which provides
 * structure to our address space.
 */

#include <stdint.h>
#include <stdio.h>
#include "stm32g0xx.h"
#include "i2c.h"
#include "uart.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

#define GPIOA_EN (1U << 0)
#define GPIOC_EN (1U << 2)

#define GPIOA_MODE (1U << 10)
#define GPIOA_MASK ~(1U << 11)

#define GPIOC_MODE ~(3 << 26)
#define GPIOA5 (1U << 5)

extern volatile uint8_t dma_transfer_complete;

int _write(int file, char *ptr, int len)
{
	(void)file;
	if (len == 0) return 0;
	while (!dma_transfer_complete);  // Wait for previous transfer to complete
	dma_transfer_complete = 0;  // Reset flag for next transfer
	dma1_transmit((uint32_t)ptr, (uint32_t)len);
	while (!dma_transfer_complete); // Wait for current transfer to complete
	return len;
}

int main(void)
{
	uart2_tx_init();
	dma1_channel1_init();
	printf("Hello, World!\n\r");
	/**
	 * In order to use GPIO (and other peripherals), we need to enable clock access
	 * to the GPIO peripherals
	 */
	RCC->IOPENR |= GPIOA_EN;
	RCC->IOPENR |= GPIOC_EN;

	/**
	 * Here, we define the pinmode: Input, Output, Alternate function etc.
	 */
	GPIOA->MODER |= GPIOA_MODE;
	GPIOA->MODER &= GPIOA_MASK;

	/* GPIOC configuration */
	GPIOC->MODER &= GPIOC_MODE; //configure c13 as output
	GPIOC->PUPDR &= ~(3U << 26); //ensure no pull up or pull down

	/* Loop forever */
	for (;;) {
		/**
		 * PC13 has a pull up resistor, so it'll be active high when the button isn't pressed.
		 */
		int result = GPIOC->IDR & (1U << 13); //IDR contains the input values of GPIOC (Input Data Register)
		if (result) {
			GPIOA->ODR |= GPIOA5; //ODR contains the output values of GPIOA (Output Data Register)
		} else {
			GPIOA->ODR &= ~GPIOA5;
		}
	}
}
