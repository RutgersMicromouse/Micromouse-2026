#include "gpio.h"
#include "stm32g070xx.h"
void gpio_init(void)
{
	/* initialize LED gpio*/

    /* enable GPIOB */
    RCC->IOPENR |= (1U << 1); // Enable clock for GPIOB
    /* Set PB8 and PB9 as output */
    GPIOB->MODER &= ~(0xF << 16); // Clear mode bits for PB8 and PB9
    GPIOB->MODER |= (0x5 << 16);  // Set PB8 and PB9 to output mode (01)

    /* enable GPIOC */
    RCC->IOPENR |= (1U << 2); // Enable clock for GPIOC
    /* Set PC10 as output */
    GPIOC->MODER &= ~(0x3 << 20); // Clear mode bits for PC10
    GPIOC->MODER |= (0x1 << 20);  // Set PC10 to output mode (01)

    /* configure USER_BUTTON (PC13) as input with pull-up */
    GPIOC->MODER &= ~(0x3 << 26); // Clear mode bits for PC13 (input mode)
    GPIOC->PUPDR &= ~(0x3 << 26); // Clear pull-up/pull-down bits for PC13
    GPIOC->PUPDR |= (0x1 << 26);  // Set pull-up for PC13 (01)

    /* Unlock RTC domain for PC14 and PC15 */
    RCC->APBENR1 |= RCC_APBENR1_PWREN; // Enable PWR clock
    (void)RCC->APBENR1;                // Dummy read to ensure clock is stable

    PWR->CR1 |= PWR_CR1_DBP;           // Unlock RTC domain
    while (!(PWR->CR1 & PWR_CR1_DBP)); // Wait for DBP bit to be set

    /* Reset Backup Domain to ensure LSE is disabled and pins are GPIO */
    RCC->BDCR |= RCC_BDCR_BDRST;
    RCC->BDCR &= ~RCC_BDCR_BDRST;

    /* Ensure LSE is OFF (though BDRST should have handled it) */
    RCC->BDCR &= ~RCC_BDCR_LSEON;

    /* configure buttons STATE and CONFIRM */
    GPIOC->MODER &= ~(0xF << 28); // Clear mode bits for PC14 and PC15 (input mode)
    GPIOC->PUPDR &= ~(0xF << 28); // Clear pull-up/pull-down bits for PC14 and PC15
    GPIOC->PUPDR |= (0x5 << 28);  // Set pull-up for PC14 and PC15 (01)
}

void set_led(gpio_pin_t pin, int state)
{
    switch (pin) {
        case GPIO_RED:
            if (state) {
                GPIOB->BSRR = (1U << 8); // Set PB8
            } else {
                GPIOB->BSRR = (1U << (8 + 16)); // Reset PB8
            }
            break;
        case GPIO_GREEN:
            if (state) {
                GPIOB->BSRR = (1U << 9); // Set PB9
            } else {
                GPIOB->BSRR = (1U << (9 + 16)); // Reset PB9
            }
            break;
        case GPIO_BLUE:
            if (state) {
                GPIOC->BSRR = (1U << 10); // Set PC10
            } else {
                GPIOC->BSRR = (1U << (10 + 16)); // Reset PC10
            }
            break;
        default:
            break;
    }
}

void poll_button(gpio_pin_t button, int* state)
{
    switch (button) {
        case GPIO_CONFIRM:
            /* PC14 uses pull-up, so active-low: pin LOW = pressed (1), pin HIGH = released (0) */
            *state = (GPIOC->IDR & (1U << 14)) ? 0 : 1; // Read PC14
            break;
        case GPIO_STATE:
            /* PC15 uses pull-up, so active-low: pin LOW = pressed (1), pin HIGH = released (0) */
            *state = (GPIOC->IDR & (1U << 15)) ? 0 : 1; // Read PC15
            break;
        case GPIO_USER_BUTTON:
            /* PC13 uses pull-up, so active-low: pin LOW = pressed (1), pin HIGH = released (0) */
            *state = (GPIOC->IDR & (1U << 13)) ? 0 : 1; // Read PC13
            break;
        default:
            *state = -1; // Invalid button
            break;
    }
}
