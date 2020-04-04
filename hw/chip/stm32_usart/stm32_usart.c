#include "stm32_usart.h"

#define USART1_BASE         (PERIPH_BASE + 0x11000)
#define USART2_BASE         (PERIPH_BASE + 0x4400)
#define USART3_BASE         (PERIPH_BASE + 0x4800)
#define UART4_BASE          (PERIPH_BASE + 0x4c00)
#define UART5_BASE          (PERIPH_BASE + 0x5000)
#define USART6_BASE         (PERIPH_BASE + 0x11400)

usart_t USART1 = reinterpret_cast<usart_t>(USART1_BASE);
usart_t USART2 = reinterpret_cast<usart_t>(USART2_BASE);
usart_t USART3 = reinterpret_cast<usart_t>(USART3_BASE);
usart_t UART4  = reinterpret_cast<usart_t>(UART4_BASE);
usart_t UART5  = reinterpret_cast<usart_t>(UART5_BASE);
usart_t USART6 = reinterpret_cast<usart_t>(USART6_BASE);

void
UsartPeriph::enable() volatile
{
    CR1 |= USART_CR1_UE;
}

void
UsartPeriph::disable() volatile
{
    CR1 &= ~USART_CR1_UE;
}

void
UsartPeriph::send(const uint8_t byte) volatile
{
    /* Wait for TXE bit to go high */
    while ((SR & USART_SR_TXE) == 0) { }
    DR = byte;
}

/*
 * Call this before disabling the usart to
 * ensure it finishes any in progress transmites.
 */
void
UsartPeriph::finish_send() volatile
{
    /* Wait for TC bit to go high before disabling USART */
    while ((SR & USART_SR_TC) == 0);
}

/*
 * Ready a usart for use.
 */
void
UsartPeriph::init() volatile
{
    enable();

    /* Set BRR to 416.6875 */
    BRR |= (416u << USART_BRR_MANT_SHIFT) & USART_BRR_MANT;
    BRR |= 11u & USART_BRR_FRAC;

    /* Set CR1:
     *  - Enable transmitter
     *  - Enable receiver
     */
    CR1 |= USART_CR1_TE | USART_CR1_RE;

    /* Set GTPR (Not yet implemented in QEMU) */
    //USART1->GTPR |= 0x1;

    /* Everything else at defaults */

    disable();
}

