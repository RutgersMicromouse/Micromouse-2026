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
#include "gpio.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

#define GPIOA_EN   (1U << 0)
#define GPIOA_MODE (1U << 10)
#define GPIOA_MASK ~(1U << 11)

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
    gpio_init();
    uart2_tx_init();
    dma1_channel1_init();
}

int main(void)
{
    initialize_system();
    printf("Hello, World!\r\n");

    /* Enable clock and configure GPIOA5 as output (not handled in gpio_init) */
    RCC->IOPENR |= GPIOA_EN;
    GPIOA->MODER |= GPIOA_MODE;
    GPIOA->MODER &= GPIOA_MASK;

    /* NOTE: GPIOB, GPIOC clocks and pin modes are fully configured in gpio_init().
     * Do not re-configure them here — it would overwrite pull-up settings on PC14/PC15. */

    int ledindex    = 0;
    int buttonstate = 0;

    for (;;) {
        /* Reset button state at the top of each cycle so we always wait for a fresh press */
        buttonstate = 0;

        if (ledindex % 3 == 0) {
            set_led(GPIO_RED,   1);
            set_led(GPIO_GREEN, 0);
            set_led(GPIO_BLUE,  0);
        } else if (ledindex % 3 == 1) {
            set_led(GPIO_RED,   0);
            set_led(GPIO_GREEN, 1);
            set_led(GPIO_BLUE,  0);
        } else {
            set_led(GPIO_RED,   0);
            set_led(GPIO_GREEN, 0);
            set_led(GPIO_BLUE,  1);
        }

        /* Poll until button is pressed */
        while (buttonstate != 1) {
            poll_button(GPIO_USER_BUTTON, &buttonstate);
			printf("button state: %d\n\r", buttonstate);
			for(int i = 0; i < 0xFFFF; i++);
        }

        /* Debounce: delay after detecting press, then wait for release */
        for (int i = 0; i < 0xFFFFF; i++);
        do {
            poll_button(GPIO_USER_BUTTON, &buttonstate);
        } while (buttonstate == 1); // wait for button to be released before advancing

        ledindex++;
    }
}