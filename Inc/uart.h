#ifndef UART_H_
#define UART_H_
#include <stdint.h>

#include "stm32g070xx.h"

void uart2_tx_init(void);
void uart2_write(int ch);
void DMA1_Channel1_IRQHandler(void);

void dma1_channel1_init(void);
void dma1_transmit(uint32_t src, uint32_t len);

#define SR_RXNE			(1U<<5)
#define HISR_TCIF6		(1U<<21)
#define HIFCR_CTCIF6    (1U<<21)
#endif /* UART_H_ */
