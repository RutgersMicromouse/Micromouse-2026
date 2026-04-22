/**
 * This is supposed to show how to configure GPIO
 * and introduce the stm32g070xx.h header, which provides
 * structure to our address space.
 */

#include <stdint.h>
#include <stdio.h>
#include "stm32g070xx.h"
#include "i2c.h"
#include "uart.h"
#include "timer.h"
#include "delay.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

#define GPIOA_EN   (1U << 0)
#define GPIOA_MODE (1U << 10)
#define GPIOA_MASK ~(1U << 11)
#define GPIOA5     (1U << 5)

extern volatile uint8_t dma_transfer_complete;

int _write(int file, char *ptr, int len)
{
    (void)file;
    if (len == 0) return 0;
    while (!dma_transfer_complete);  // Wait for previous transfer to complete
    dma_transfer_complete = 0;       // Reset flag for next transfer
    dma1_transmit((uint32_t)ptr, (uint32_t)len);
    while (!dma_transfer_complete);  // Wait for current transfer to complete
    return len;
}

void initialize_system(void)
{
    timer_init();
    uart2_tx_init();
    dma1_channel1_init();
}

int main(void)
{
    initialize_system();
    printf("System Initialized\r\n");

    /* Enable clock and configure GPIOA5 as output (not handled in gpio_init) */
    RCC->IOPENR |= GPIOA_EN;
    GPIOA->MODER |= GPIOA_MODE;
    GPIOA->MODER &= GPIOA_MASK;

    for (;;) {

		printf("hello\n\r");
        /* Debounce: wait 50ms, then wait for release */
        delay_ms(1000);
    }
}
