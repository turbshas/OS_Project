#include "stm32_rcc.h"

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

#define RCC_BDCR_RTCEN          (1u << 15)
#define RCC_BDCR_RTCSEL         0x300

#define RCC_BDCR_RTCSEL_SHIFT   8u

volatile RccPeriph *const RCC = reinterpret_cast<volatile RccPeriph *>(RCC_BASE);

void
RccPeriph::periph_cmd(volatile uint32_t *const reg, const uint32_t periph, const bool state) volatile
{
    if (state) {
        *reg = *reg | periph;
    } else {
        *reg = *reg & ~periph;
    }
}

void
RccPeriph::AHB1_periph_cmd(const enum AHB1_periphs periph, const bool state) volatile
{
    periph_cmd(&AHB1ENR, periph, state);
}

void
RccPeriph::AHB1_LP_periph_cmd(const enum AHB1_periphs periph, const bool state) volatile
{
    periph_cmd(&AHB1LPENR, periph, state);
}

void
RccPeriph::AHB1_reset_cmd(const enum AHB1_periphs periph, const bool state) volatile
{
    periph_cmd(&AHB1RSTR, periph, state);
}

void
RccPeriph::AHB2_periph_cmd(const enum AHB2_periphs periph, const bool state) volatile
{
    periph_cmd(&AHB2ENR, periph, state);
}

void
RccPeriph::AHB2_LP_periph_cmd(const enum AHB2_periphs periph, const bool state) volatile
{
    periph_cmd(&AHB2LPENR, periph, state);
}

void
RccPeriph::AHB2_reset_cmd(const enum AHB2_periphs periph, const bool state) volatile
{
    periph_cmd(&AHB2RSTR, periph, state);
}

void
RccPeriph::AHB3_periph_cmd(const enum AHB3_periphs periph, const bool state) volatile
{
    periph_cmd(&AHB3ENR, periph, state);
}

void
RccPeriph::AHB3_LP_periph_cmd(const enum AHB3_periphs periph, const bool state) volatile
{
    periph_cmd(&AHB3LPENR, periph, state);
}

void
RccPeriph::AHB3_reset_cmd(const enum AHB3_periphs periph, const bool state) volatile
{
    periph_cmd(&AHB3RSTR, periph, state);
}

void
RccPeriph::APB1_periph_cmd(const enum APB1_periphs periph, const bool state) volatile
{
    periph_cmd(&APB1ENR, periph, state);
}

void
RccPeriph::APB1_LP_periph_cmd(const enum APB1_periphs periph, const bool state) volatile
{
    periph_cmd(&APB1LPENR, periph, state);
}

void
RccPeriph::APB1_reset_cmd(const enum APB1_periphs periph, const bool state) volatile
{
    periph_cmd(&APB1RSTR, periph, state);
}

void
RccPeriph::APB2_periph_cmd(const enum APB2_periphs periph, const bool state) volatile
{
    periph_cmd(&APB2ENR, periph, state);
}

void
RccPeriph::APB2_LP_periph_cmd(const enum APB2_periphs periph, const bool state) volatile
{
    periph_cmd(&APB2LPENR, periph, state);
}

void
RccPeriph::APB2_reset_cmd(const enum APB2_periphs periph, const bool state) volatile
{
    periph_cmd(&APB2RSTR, periph, state);
}

void
RccPeriph::init() volatile
{
    /* Disable all interrupts */
    CIR = 0x00000000;

    /* Set PLL CFG register:
     *  - PLL source to HSI
     *  - Main PLL divider for VCO set to 8 (VCO = HSI * 192 / 8)
     *  - Main PLL multiplier for VCO set to 192
     *  - Main PLL divider for main system clock to 6 (SYSCLCK = VCO / 6)
     *  - Main PLL divider for USB OTG FS, SDIO, and RNG to 8 (OTG FS, SDIO, RNG = VCO / 8)
     */
    PLLCFGR = PLLCFGR & ~RCC_PLLCFGR_PLLSRC;
    PLLCFGR = (0x8  << RCC_PLLCFGR_PLLQ_SHIFT)
                 | (0x2  << RCC_PLLCFGR_PLLP_SHIFT)
                 | (0xc0 << RCC_PLLCFGR_PLLN_SHIFT)
                 | (0x8  << RCC_PLLCFGR_PLLM_SHIFT);

    /* Set Control register:
     *  - Enable HSI clock
     *  - Enable PLL clock
     *  - Disable HSE clock
     *  - Do not bypass HSE with external clock
     *  - Disable clock security system
     */
    CR = CR | (RCC_CR_HSION | RCC_CR_PLLON);
    CR = CR & ~(RCC_CR_HSEON | RCC_CR_HSEBYP | RCC_CR_CSSON);

    /* Wait for PLL to be ready */
    while ((CR & RCC_CR_PLLRDY) == 0) { }

    /* Set CFG register:
     *  - PLL selected as system clock
     *  - PLLI2S used for I2S clock
     *  - AHB prescaler set to 1
     *  - APB1 (low speed) prescaler set to 4
     *  - APB2 (high speed) prescaler set to 2
     */
    CFGR = (0x5 << RCC_CFGR_PPRE1_SHIFT) | (0x4 << RCC_CFGR_PPRE2_SHIFT) | (0x2 << RCC_CFGR_SW_SHIFT);

    /* Wait for PLL to be used as system clock */
    while (((CFGR & RCC_CFGR_SWS) >> RCC_CFGR_SWS_SHIFT) != 0x2) { }

    /* Enable RTC, set clock to LSE */
    BDCR = BDCR | (RCC_BDCR_RTCEN | (0x1 << RCC_BDCR_RTCSEL_SHIFT));

    /* Enable USARTs */
    APB1_periph_cmd(UART5, true);
    APB1_periph_cmd(UART4, true);
    APB1_periph_cmd(USART3, true);
    APB1_periph_cmd(USART2, true);
    APB2_periph_cmd(USART6, true);
    APB2_periph_cmd(USART1, true);

    /* Enable DMAs */
    AHB1_periph_cmd(DMA1, true);
    AHB1_periph_cmd(DMA2, true);
}

