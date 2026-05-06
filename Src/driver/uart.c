#include "uart.h"
#include "stm32g070xx.h"

#define GPIOAEN             (1U<<0)
#define UART2EN             (1U<<17)
#define DMA1EN              (1U<<0)

#define CR1_TE              (1U<<3)
#define CR1_RE              (1U<<2)
#define CR1_UE              (1U<<0)
#define ISR_TXE             (1U<<7)
#define CR1_RXNEIE          (1U<<5)

#define DMA_MEM_INC         (1U<<7)
#define DMA_DIR_MEM_TO_PERIPH (1U<<4)
#define DMA_CR_TCIE         (1U<<1)
#define DMA_CR_EN           (1U<<0)
#define UART_CR3_DMAT       (1U<<7)

#define SYS_FREQ            16000000
#define APB1_CLK            SYS_FREQ
#define UART_BAUDRATE       115200

static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate);
static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate);

volatile uint8_t dma_transfer_complete = 1;  // Flag to indicate DMA transfer completion, initialized to 1 (ready for first transfer)

/**
 * @brief USART2 DMA1 Channel 1 interrupt handler. This function is
 * called when a DMA transfer for USART2 TX completes. It clears
 * the transfer complete flag, sets the dma_transfer_complete flag
 * to indicate the transfer is done, and disables the DMA channel
 * to allow for reconfiguration for the next transfer.
 */
void DMA1_Channel1_IRQHandler(void)
{
    /* Clear transfer complete flag */
    DMA1->IFCR |= (1U<<1);  // CTCIF1
    dma_transfer_complete = 1;
    /* Disable channel so it can be reconfigured next transfer */
    DMA1_Channel1->CCR &= ~DMA_CR_EN;
}

/**
 * @brief Write a single character to USART2. This function waits until
 * the transmit data register is empty, then writes the character to the
 * TDR register to initiate transmission.
 * @param ch The character to transmit (only the least significant byte is used)
 */
void uart2_write(int ch)
{
    /* Make sure the transmit data register is empty */
    while (!(USART2->ISR & ISR_TXE));

    /* Write to transmit data register */
    USART2->TDR = (ch & 0xFF);
}


/**
 * @brief Initialize DMA1 Channel 1 for USART2 TX DMA transfers
 * This function configures DMA1 Channel 1 to handle memory-to-peripheral 
 * transfers for USART2 TX. It sets up the DMAMUX to route USART2 TX requests 
 * to Channel 1, configures the DMA channel for memory increment mode, 
 * memory-to-peripheral direction, and enables transfer
 */
void dma1_channel1_init(void)
{
    /* Enable clock access to DMA1 and DMAMUX */
    RCC->AHBENR |= DMA1EN;

    /* Configure DMAMUX for USART2_TX (Request 53) on Channel 1 */
    DMAMUX1_Channel0->CCR = 53;

    /* Configure Channel 1:
     * - Memory increment enabled
     * - Direction: Memory to Peripheral
     * - Transfer complete interrupt enabled
     */
    DMA1_Channel1->CCR &= ~DMA_CR_EN; // Ensure channel is disabled
    DMA1_Channel1->CCR = DMA_MEM_INC | DMA_DIR_MEM_TO_PERIPH | DMA_CR_TCIE;
    
    /* Set the peripheral address once (USART2 TDR) */
    DMA1_Channel1->CPAR = (uint32_t)&USART2->TDR;

    /* Enable UART2 transmitter DMA request */
    USART2->CR3 |= UART_CR3_DMAT;

    /* Enable DMA interrupt in NVIC */
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

/**
 * @brief Triggers a new DMA transfer. Assumes channel is currently disabled.
 */
void dma1_transmit(uint32_t src, uint32_t len)
{
    /* Clear transfer complete flag */
    DMA1->IFCR |= (1U << 1); // CTCIF1

    /* Update memory address and length */
    DMA1_Channel1->CMAR = src;
    DMA1_Channel1->CNDTR = len;

    /* Enable the channel to start the transfer */
    DMA1_Channel1->CCR |= DMA_CR_EN;
}

void uart2_tx_init(void)
{
    /****************Configure uart gpio pin***************/
    /* Enable clock access to gpioa */
    RCC->IOPENR |= GPIOAEN;

    /* Set PA2 mode to alternate function mode */
    GPIOA->MODER &= ~(3U<<4);
    GPIOA->MODER |= (2U<<4);

    /* Set PA2 alternate function type to UART_TX (AF1) */
    GPIOA->AFR[0] &= ~(0xF<<8);
    GPIOA->AFR[0] |= (1U<<8);

    /****************Configure uart module ***************/
    /* Enable clock access to uart2 */
    RCC->APBENR1 |= UART2EN;

    /* Configure baudrate */
    uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);

    /* Configure the transfer direction */
    USART2->CR1 = CR1_TE;

    /* Enable uart module */
    USART2->CR1 |= CR1_UE;
}

static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate)
{
    USARTx->BRR = compute_uart_bd(PeriphClk, BaudRate);
}

static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate)
{
    return ((PeriphClk + (BaudRate/2U))/BaudRate);
}
