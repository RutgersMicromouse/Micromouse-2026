#ifndef GPIO_H
#define GPIO_H

typedef enum {
    GPIO_RED, // PB8
    GPIO_GREEN, //PB9
    GPIO_BLUE, //PC10
    GPIO_CONFIRM, //PC14
    GPIO_STATE, //PC15
    GPIO_USER_BUTTON, //PC13
} gpio_pin_t;

void gpio_init(void);
void set_led(gpio_pin_t pin, int state);
void poll_button(gpio_pin_t button, int* state);
#endif
