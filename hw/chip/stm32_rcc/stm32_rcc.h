#ifndef _RCC_H
#define _RCC_H

#include "chip_common.h"

#define AHB1_OTGHSULPI  (1u << 30)
#define AHB1_OTGHS      (1u << 29)
#define AHB1_ETHPTP     (1u << 28)
#define AHB1_ETHRX      (1u << 27)
#define AHB1_ETHTX      (1u << 26)
#define AHB1_ETHMAC     (1u << 25)
#define AHB1_DMA2       (1u << 22)
#define AHB1_DMA1       (1u << 21)
#define AHB1_BKPSRAM    (1u << 18)
#define AHB1_SRAM2      (1u << 17)
#define AHB1_SRAM1      (1u << 16)
#define AHB1_FLITF      (1u << 15)
#define AHB1_CRC        (1u << 12)
#define AHB1_GPIOI      (1u << 8)
#define AHB1_GPIOH      (1u << 7)
#define AHB1_GPIOG      (1u << 6)
#define AHB1_GPIOF      (1u << 5)
#define AHB1_GPIOE      (1u << 4)
#define AHB1_GPIOD      (1u << 3)
#define AHB1_GPIOC      (1u << 2)
#define AHB1_GPIOB      (1u << 1)
#define AHB1_GPIOA      (1u << 0)

#define AHB2_OTGFS      (1u << 7)
#define AHB2_RNG        (1u << 6)
#define AHB2_HASH       (1u << 5)
#define AHB2_CRYP       (1u << 4)
#define AHB2_DCMI       (1u << 0)

#define AHB3_FSMC       (1u << 0)

#define APB1_DAC        (1u << 29)
#define APB1_PWR        (1u << 28)
#define APB1_CAN2       (1u << 26)
#define APB1_CAN1       (1u << 25)
#define APB1_I2C3       (1u << 23)
#define APB1_I2C2       (1u << 22)
#define APB1_I2C1       (1u << 21)
#define APB1_UART5      (1u << 20)
#define APB1_UART4      (1u << 19)
#define APB1_USART3     (1u << 18)
#define APB1_USART2     (1u << 17)
#define APB1_SPI3       (1u << 15)
#define APB1_SPI2       (1u << 14)
#define APB1_WWDG       (1u << 11)
#define APB1_TIM14      (1u << 8)
#define APB1_TIM13      (1u << 7)
#define APB1_TIM12      (1u << 6)
#define APB1_TIM7       (1u << 5)
#define APB1_TIM6       (1u << 4)
#define APB1_TIM5       (1u << 3)
#define APB1_TIM4       (1u << 2)
#define APB1_TIM3       (1u << 1)
#define APB1_TIM2       (1u << 0)

#define APB2_TIM11      (1u << 18)
#define APB2_TIM10      (1u << 17)
#define APB2_TIM9       (1u << 16)
#define APB2_SYSCFG     (1u << 14)
#define APB2_SPI1       (1u << 12)
#define APB2_SDIO       (1u << 11)
#define APB2_ADC        (1u << 8)
#define APB2_USART6     (1u << 5)
#define APB2_USART1     (1u << 4)
#define APB2_TIM8       (1u << 1)
#define APB2_TIM1       (1u << 0)

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

/*
 * Periph commands enable/disable each peripheral
 * Low-Power periph commands enable/disable each peripheral in low power mode
 * Reset commands reset each peripheral
 */
void rcc_AHB1_periph_cmd(const uint32_t periph, const int state);
void rcc_AHB1_LP_periph_cmd(const uint32_t periph, const int state);
void rcc_AHB1_reset_cmd(const uint32_t periph, const int state);

void rcc_AHB2_periph_cmd(const uint32_t periph, const int state);
void rcc_AHB2_LP_periph_cmd(const uint32_t periph, const int state);
void rcc_AHB2_reset_cmd(const uint32_t periph, const int state);

void rcc_AHB3_periph_cmd(const uint32_t periph, const int state);
void rcc_AHB3_LP_periph_cmd(const uint32_t periph, const int state);
void rcc_AHB3_reset_cmd(const uint32_t periph, const int state);

void rcc_APB1_periph_cmd(const uint32_t periph, const int state);
void rcc_APB1_LP_periph_cmd(const uint32_t periph, const int state);
void rcc_APB1_reset_cmd(const uint32_t periph, const int state);

void rcc_APB2_periph_cmd(const uint32_t periph, const int state);
void rcc_APB2_LP_periph_cmd(const uint32_t periph, const int state);
void rcc_APB2_reset_cmd(const uint32_t periph, const int state);

void rcc_init(void);

#endif /* _RCC_H */

