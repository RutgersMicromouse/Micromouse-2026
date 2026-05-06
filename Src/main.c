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

void init_adc_continuous_PA0(void)
{
    /* 1. Enable Clocks */
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;    // Enable GPIOA clock
    RCC->APBENR2 |= RCC_APBENR2_ADCEN;    // Enable ADC clock [4]

    /* 2. Configure PA0 as Analog */
    GPIOA->MODER |= GPIO_MODER_MODE0;     // Set PA0 to Analog mode (11) [2]

    /* 3. Initialize ADC (Regulator & Calibration) */
    ADC1->CR |= ADC_CR_ADVREGEN;          // Enable voltage regulator [5]
    for(volatile int i=0; i<1000; i++);   // Wait for regulator startup
    
    ADC1->CR |= ADC_CR_ADCAL;             // Start calibration [6]
    while (ADC1->CR & ADC_CR_ADCAL);      // Wait for calibration to finish [7]

    /* 4. Enable ADC */
    ADC1->ISR |= ADC_ISR_ADRDY;           // Clear ready flag
    ADC1->CR |= ADC_CR_ADEN;              // Enable ADC [8]
    while (!(ADC1->ISR & ADC_ISR_ADRDY)); // Wait until ready [8]

    /* 5. Configure Continuous Mode and Select Channel */
    ADC1->CFGR1 |= ADC_CFGR1_CONT;        // Enable Continuous conversion mode [3]
    
    // Select Channel 0 (PA0)
    ADC1->CHSELR = ADC_CHSELR_CHSEL0;     // [9]
    while (!(ADC1->ISR & ADC_ISR_CCRDY)); // Wait for channel config to be applied [10]

    /* 6. Start the continuous conversion */
    ADC1->CR |= ADC_CR_ADSTART;           // [11]
}


uint16_t read_ir_sensor(void) {
	GPIOA->ODR |= GPIOA5; // Turn on IR LED
    // while (!(ADC1->ISR & ADC_ISR_EOC)); // Wait for conversion complete [12]
	while (!(ADC1->ISR & ADC_ISR_EOC)); // Wait for conversion complete [12]
	int adc_value = ADC1->DR; // Read ADC value (also clears EOC flag)
	GPIOA->ODR &= ~GPIOA5; // Turn off IR LED
    return adc_value;      // Read data (automatically clears EOC) [12]
}

void init_pa5_gpio(void) {
	/* Enable clock for GPIOA */
	// RCC->IOPENR |= GPIOA_EN;

	/* Set PA5 as output */
	GPIOA->MODER &= ~(3U << 10); // Clear mode bits for PA5
	GPIOA->MODER |= (1U << 10);  // Set PA5 to output mode (01)
}

int main(void)
{
	uart2_tx_init();
	dma1_channel1_init();
	printf("\n\rInitialized uart\n\r"); //PB11 emitter, PA0 receiver
	init_pa5_gpio();
	init_adc_continuous_PA0();
	printf("Started ADC in continuous mode on PA0\n\r");
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
	// int prev_result = 0;
	/* Loop forever */
	for (;;) {
		/**
		 * PC13 has a pull up resistor, so it'll be active high when the button isn't pressed.
		 */
		// int result = GPIOC->IDR & (1U << 13); //IDR contains the input values of GPIOC (Input Data Register)
		// if (result) {
		// 	GPIOA->ODR |= GPIOA5; //ODR contains the output values of GPIOA (Output Data Register)
		// } else {
		// 	GPIOA->ODR &= ~GPIOA5;
		// }
		// if(result != prev_result) {
		// 	printf("Button state changed: %s\n\r", result ? "Released" : "Pressed");
		// }
		printf("IR Sensor Value: %u\n\r", read_ir_sensor());
		for(volatile int i=0; i<0xFFFF; i++); // Simple delay
		// prev_result = result;
	}
}
