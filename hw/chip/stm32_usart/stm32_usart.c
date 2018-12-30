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

static int
usart_enable(usart_t usart)
{
    usart->CR1 |= USART_CR1_UE;

    return 0;
}

static int
usart_disable(usart_t usart)
{
    usart->CR1 &= ~USART_CR1_UE;

    return 0;
}

static inline void
usart_send(usart_t usart, const uint8_t byte)
{
    /* Wait for TXE bit to go high */
    while ((usart->SR & USART_SR_TXE) == 0) { }
    usart->DR = byte;
}

static inline void
usart_finish_send(usart_t usart)
{
    /* Wait for TC bit to go high before disabling USART */
    while ((usart->SR & USART_SR_TC) == 0);
}

int
usart_send_byte(usart_t usart, const char byte)
{
    if (byte > 0xff) {
        return -1;
    }

    if (usart_enable(usart) < 0) {
        return -1;
    }

    usart_send(usart, byte);
    usart_finish_send(usart);

    usart_disable(usart);

    return 0;
}

int
usart_send_string(usart_t usart, const char *const str, const uint8_t len)
{
    int count = 0;

    if (usart_enable(usart) < 0) {
        return -1;
    }

    for (int i = 0; i < len; i++) {
        if (str[i] > 0xff) {
            return count;
        }
        usart_send(usart, str[i]);
        count++;
    }

    usart_finish_send(usart);
    usart_disable(usart);

    return count;
}

static void
usart_init(usart_t usart)
{
    usart_enable(usart);

    /* Set BRR to 416.6875 */
    usart->BRR |= (416u << USART_BRR_MANT_SHIFT) & USART_BRR_MANT;
    usart->BRR |= 11u & USART_BRR_FRAC;

    /* Set CR1 */
    usart->CR1 |= USART_CR1_TE | USART_CR1_RE;

    /* Set GTPR (Not yet implemented in QEMU) */
    //USART1->GTPR |= 0x1;

    /* Everything else at defaults */

    usart_disable(usart);
}

void
USART_Init(void)
{
    usart_init(USART3);
}

