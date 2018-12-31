#include "stm32_usart.h"

#define USART1_BASE         (PERIPH_BASE + 0x11000)
#define USART2_BASE         (PERIPH_BASE + 0x4400)
#define USART3_BASE         (PERIPH_BASE + 0x4800)
#define UART4_BASE          (PERIPH_BASE + 0x4c00)
#define UART5_BASE          (PERIPH_BASE + 0x5000)
#define USART6_BASE         (PERIPH_BASE + 0x11400)

usart_t USART1 = (void *)USART1_BASE;
usart_t USART2 = (void *)USART2_BASE;
usart_t USART3 = (void *)USART3_BASE;
usart_t UART4  = (void *)UART4_BASE;
usart_t UART5  = (void *)UART5_BASE;
usart_t USART6 = (void *)USART6_BASE;

void
usart_enable(usart_t usart)
{
    usart->CR1 |= USART_CR1_UE;
}

void
usart_disable(usart_t usart)
{
    usart->CR1 &= ~USART_CR1_UE;
}

void
usart_send(usart_t usart, const uint8_t byte)
{
    /* Wait for TXE bit to go high */
    while ((usart->SR & USART_SR_TXE) == 0) { }
    usart->DR = byte;
}

/*
 * Call this before disabling the usart to
 * ensure it finishes any in progress transmites.
 */
void
usart_finish_send(usart_t usart)
{
    /* Wait for TC bit to go high before disabling USART */
    while ((usart->SR & USART_SR_TC) == 0);
}

/*
 * Ready a usart for use.
 */
void
usart_init(usart_t usart)
{
    usart_enable(usart);

    /* Set BRR to 416.6875 */
    usart->BRR |= (416u << USART_BRR_MANT_SHIFT) & USART_BRR_MANT;
    usart->BRR |= 11u & USART_BRR_FRAC;

    /* Set CR1:
     *  - Enable transmitter
     *  - Enable receiver
     */
    usart->CR1 |= USART_CR1_TE | USART_CR1_RE;

    /* Set GTPR (Not yet implemented in QEMU) */
    //USART1->GTPR |= 0x1;

    /* Everything else at defaults */

    usart_disable(usart);
}

