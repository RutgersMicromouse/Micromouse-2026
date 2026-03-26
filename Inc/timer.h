#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
void init_encoder_tim1(void);
uint16_t read_encoder_position(void);

#endif /* TIMER_H */
