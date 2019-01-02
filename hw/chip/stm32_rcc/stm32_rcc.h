#ifndef _RCC_H
#define _RCC_H

#include "chip_common.h"

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

#define RCC_BDCR_RTCEN          (1u << 15)
#define RCC_BDCR_RTCSEL         0x300

#define RCC_BDCR_RTCSEL_SHIFT   8u

enum rcc_state {
    RCC_STATE_RESET,
    RCC_STATE_CLOCK_DISABLE,
    RCC_STATE_CLOCK_ENABLE,
    RCC_STATE_CLOCK_LPDISABLE,
    RCC_STATE_CLOCK_LPENABLE,
    RCC_STATE_NUM
};

enum rcc_AHB1_periph {
    RCC_AHB1_OTGHSULPI,
    RCC_AHB1_OTGHS,
    RCC_AHB1_ETHPTP,
    RCC_AHB1_ETHRX,
    RCC_AHB1_ETHTX,
    RCC_AHB1_ETHMAC,
    RCC_AHB1_DMA2,
    RCC_AHB1_DMA1,
    RCC_AHB1_BKPSRAM,
    RCC_AHB1_SRAM2,
    RCC_AHB1_SRAM1,
    RCC_AHB1_FLITF,
    RCC_AHB1_CRC,
    RCC_AHB1_GPIOI,
    RCC_AHB1_GPIOH,
    RCC_AHB1_GPIOG,
    RCC_AHB1_GPIOF,
    RCC_AHB1_GPIOE,
    RCC_AHB1_GPIOD,
    RCC_AHB1_GPIOC,
    RCC_AHB1_GPIOB,
    RCC_AHB1_GPIOA,
    RCC_AHB1_NUM
};

enum rcc_AHB2_periph {
    RCC_AHB2_OTGFS,
    RCC_AHB2_RNG,
    RCC_AHB2_HASH,
    RCC_AHB2_CRYP,
    RCC_AHB2_DCMI,
    RCC_AHB2_NUM
};

enum rcc_AHB3_periph {
    RCC_AHB3_FSMC,
    RCC_AHB3_NUM
};

enum rcc_APB1_periph {
    RCC_APB1_DAC,
    RCC_APB1_PWR,
    RCC_APB1_CAN2,
    RCC_APB1_CAN1,
    RCC_APB1_I2C3,
    RCC_APB1_I2C2,
    RCC_APB1_I2C1,
    RCC_APB1_UART5,
    RCC_APB1_UART4,
    RCC_APB1_USART3,
    RCC_APB1_USART2,
    RCC_APB1_SPI3,
    RCC_APB1_SPI2,
    RCC_APB1_WWDG,
    RCC_APB1_TIM14,
    RCC_APB1_TIM13,
    RCC_APB1_TIM12,
    RCC_APB1_TIM7,
    RCC_APB1_TIM6,
    RCC_APB1_TIM5,
    RCC_APB1_TIM4,
    RCC_APB1_TIM3,
    RCC_APB1_TIM2,
    RCC_APB1_NUM
};

enum rcc_APB2_periph {
    RCC_APB2_TIM11,
    RCC_APB2_TIM10,
    RCC_APB2_TIM9,
    RCC_APB2_SYSCFG,
    RCC_APB2_SPI1,
    RCC_APB2_SDIO,
    RCC_APB2_ADC,
    RCC_APB2_USART6,
    RCC_APB2_USART1,
    RCC_APB2_TIM8,
    RCC_APB2_TIM1,
    RCC_APB2_NUM
};

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

void rcc_AHB1_set_periph_state(const enum rcc_AHB1_periph periph, const enum rcc_state state);
void rcc_AHB2_set_periph_state(const enum rcc_AHB2_periph periph, const enum rcc_state state);
void rcc_AHB3_set_periph_state(const enum rcc_AHB3_periph periph, const enum rcc_state state);
void rcc_APB1_set_periph_state(const enum rcc_APB1_periph periph, const enum rcc_state state);
void rcc_APB2_set_periph_state(const enum rcc_APB2_periph periph, const enum rcc_state state);

#endif /* _RCC_H */

