#ifndef _RCC_H
#define _RCC_H

#include "driver_common.h"

#define RCC_BASE            (PERIPH_BASE + 0x23800)

#define RCC_CR_HSION    (1u << 0)
#define RCC_CR_HSEON    (1u << 16)
#define RCC_CR_HSEBYP   (1u << 18)
#define RCC_CR_CSSON    (1u << 19)
#define RCC_CR_PLLON    (1u << 24)
#define RCC_CR_PLLRDY   (1u << 25)

#define RCC_PLLCFGR_PLLQ        0xf000000
#define RCC_PLLCFGR_PLLSRC      (1u << 22)
#define RCC_PLLCFGR_PLLP        0x30000
#define RCC_PLLCFGR_PLLN        0x7fc0
#define RCC_PLLCFGR_PLLM        0x3f

#define RCC_PLLCFGR_PLLQ_SHIFT  24u
#define RCC_PLLCFGR_PLLP_SHIFT  16u
#define RCC_PLLCFGR_PLLN_SHIFT  6u
#define RCC_PLLCFGR_PLLM_SHIFT  0

#define RCC_CFGR_SW             0x3
#define RCC_CFGR_SWS            0xc
#define RCC_CFGR_HPRE           0xf0
#define RCC_CFGR_PPRE1          0xfc00
#define RCC_CFGR_PPRE2          0xe000

#define RCC_CFGR_SW_SHIFT       0
#define RCC_CFGR_SWS_SHIFT      2u
#define RCC_CFGR_HPRE_SHIFT     4u
#define RCC_CFGR_PPRE1_SHIFT    10u
#define RCC_CFGR_PPRE2_SHIFT    13u

#define RCC_APB1ENR_UART5EN     (1u << 20)
#define RCC_APB1ENR_UART4EN     (1u << 19)
#define RCC_APB1ENR_USART3EN    (1u << 18)
#define RCC_APB1ENR_USART2EN    (1u << 17)

#define RCC_APB2ENR_USART6EN    (1u << 5)
#define RCC_APB2ENR_USART1EN    (1u << 4)

#define RCC_BDCR_RTCEN          (1u << 15)
#define RCC_BDCR_RTCSEL         0x300

#define RCC_BDCR_RTCSEL_SHIFT   8u

struct RCC_Regs {
    uint32_t CR;
    uint32_t PLLCFGR;
    uint32_t CFGR;
    uint32_t CIR;
    uint32_t AHB1RSTR;
    uint32_t AHB2RSTR;
    uint32_t AHB3RSTR;
    uint32_t rsvd0;
    uint32_t APB1RSTR;
    uint32_t APB2RSTR;
    uint32_t rsvd1;
    uint32_t rsvd2;
    uint32_t AHB1ENR;
    uint32_t AHB2ENR;
    uint32_t AHB3ENR;
    uint32_t rsvd3;
    uint32_t APB1ENR;
    uint32_t APB2ENR;
    uint32_t rsvd4;
    uint32_t rsvd5;
    uint32_t AHB1LPENR;
    uint32_t AHB2LPENR;
    uint32_t AHB3LPENR;
    uint32_t rsvd6;
    uint32_t APB1LPENR;
    uint32_t APB2LPENR;
    uint32_t rsvd7;
    uint32_t rsvd8;
    uint32_t BDCR;
    uint32_t CSR;
    uint32_t rsvd9;
    uint32_t rsvd10;
    uint32_t SSCGR;
    uint32_t PLLI2SCFGR;
};

extern volatile struct RCC_Regs *const RCC;

void RCC_Init(void);

#endif /* _RCC_H */

