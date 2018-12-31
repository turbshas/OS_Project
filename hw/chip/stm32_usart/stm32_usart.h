#ifndef _USART_H
#define _USART_H

#include "chip_common.h"

#define USART_SR_TXE    (1u << 7)
#define USART_SR_TC     (1u << 6)

#define USART_BRR_FRAC 0xf
#define USART_BRR_MANT 0xfff0
#define USART_BRR_MANT_SHIFT 4u

#define USART_CR1_UE    (1u << 13)
#define USART_CR1_M     (1u << 12)
#define USART_CR1_TE    (1u << 3)
#define USART_CR1_RE    (1u << 2)
#define USART_CR1_SBK   (1u << 0)

#define USART_CR2_STOP  (3u << 12)

#define USART_GTPR_PSC  0xff
#define USART_GTPR_GT   0xff00
#define USART_GTPR_GT_SHIFT 8u

struct USART_Regs {
    uint32_t SR;
    uint32_t DR;
    uint32_t BRR;
    uint32_t CR1;
    uint32_t CR2;
    uint32_t CR3;
    uint32_t GTPR;
};

typedef volatile struct USART_Regs *const usart_t;

extern usart_t USART1;
extern usart_t USART2;
extern usart_t USART3;
extern usart_t UART4;
extern usart_t UART5;
extern usart_t USART6;

void usart_enable(usart_t usart);
void usart_disable(usart_t usart);
void usart_send(usart_t, const uint8_t byte);
void usart_finish_send(usart_t usart);
void usart_init(usart_t usart);

#endif /* _USART_H */

