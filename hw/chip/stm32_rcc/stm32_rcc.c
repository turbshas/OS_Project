#include "stm32_rcc.h"

#define RCC_BASE            (PERIPH_BASE + 0x23800)

volatile struct RCC_Regs *const RCC = (void *)RCC_BASE;

void
rcc_AHB1_periph_cmd(const uint32_t periph, const int state)
{
    if (state) {
        RCC->AHB1ENR |= periph;
    } else {
        RCC->AHB1ENR &= ~periph;
    }
}

void
rcc_AHB1_LP_periph_cmd(const uint32_t periph, const int state)
{
    if (state) {
        RCC->AHB1LPENR |= periph;
    } else {
        RCC->AHB1LPENR &= ~periph;
    }
}

void
rcc_AHB1_reset_cmd(const uint32_t periph, const int state)
{
    if (state) {
        RCC->AHB1RSTR |= periph;
    } else {
        RCC->AHB1RSTR &= ~periph;
    }
}

void
rcc_AHB2_periph_cmd(const uint32_t periph, const int state)
{
    if (state) {
        RCC->AHB2ENR |= periph;
    } else {
        RCC->AHB2ENR &= ~periph;
    }
}

void
rcc_AHB2_LP_periph_cmd(const uint32_t periph, const int state)
{
    if (state) {
        RCC->AHB2LPENR |= periph;
    } else {
        RCC->AHB2LPENR &= ~periph;
    }
}

void
rcc_AHB2_reset_cmd(const uint32_t periph, const int state)
{
    if (state) {
        RCC->AHB2RSTR |= periph;
    } else {
        RCC->AHB2RSTR &= ~periph;
    }
}

void
rcc_AHB3_periph_cmd(const uint32_t periph, const int state)
{
    if (state) {
        RCC->AHB3ENR |= periph;
    } else {
        RCC->AHB3ENR &= ~periph;
    }
}

void
rcc_AHB3_LP_periph_cmd(const uint32_t periph, const int state)
{
    if (state) {
        RCC->AHB3LPENR |= periph;
    } else {
        RCC->AHB3LPENR &= ~periph;
    }
}

void
rcc_AHB3_reset_cmd(const uint32_t periph, const int state)
{
    if (state) {
        RCC->AHB3RSTR |= periph;
    } else {
        RCC->AHB3RSTR &= ~periph;
    }
}

void
rcc_APB1_periph_cmd(const uint32_t periph, const int state)
{
    if (state) {
        RCC->APB1ENR |= periph;
    } else {
        RCC->APB1ENR &= ~periph;
    }
}

void
rcc_APB1_LP_periph_cmd(const uint32_t periph, const int state)
{
    if (state) {
        RCC->APB1LPENR |= periph;
    } else {
        RCC->APB1LPENR &= ~periph;
    }
}

void
rcc_APB1_reset_cmd(const uint32_t periph, const int state)
{
    if (state) {
        RCC->APB1RSTR |= periph;
    } else {
        RCC->APB1RSTR &= ~periph;
    }
}

void
rcc_APB2_periph_cmd(const uint32_t periph, const int state)
{
    if (state) {
        RCC->APB2ENR |= periph;
    } else {
        RCC->APB2ENR &= ~periph;
    }
}

void
rcc_APB2_LP_periph_cmd(const uint32_t periph, const int state)
{
    if (state) {
        RCC->APB2LPENR |= periph;
    } else {
        RCC->APB2LPENR &= ~periph;
    }
}

void
rcc_APB2_reset_cmd(const uint32_t periph, const int state)
{
    if (state) {
        RCC->APB2RSTR |= periph;
    } else {
        RCC->APB2RSTR &= ~periph;
    }
}

void
rcc_init(void)
{
    /* Disable all interrupts */
    RCC->CIR = 0x00000000;

    /* Set PLL CFG register:
     *  - PLL source to HSI
     *  - Main PLL divider for VCO set to 8 (VCO = HSI * 192 / 8)
     *  - Main PLL multiplier for VCO set to 192
     *  - Main PLL divider for main system clock to 6 (SYSCLCK = VCO / 6)
     *  - Main PLL divider for USB OTG FS, SDIO, and RNG to 8 (OTG FS, SDIO, RNG = VCO / 8)
     */
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC;
    RCC->PLLCFGR = (0x8  << RCC_PLLCFGR_PLLQ_SHIFT)
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
    RCC->CR |= RCC_CR_HSION | RCC_CR_PLLON;
    RCC->CR &= ~(RCC_CR_HSEON | RCC_CR_HSEBYP | RCC_CR_CSSON);

    /* Wait for PLL to be ready */
    while ((RCC->CR & RCC_CR_PLLRDY) == 0) { }

    /* Set CFG register:
     *  - PLL selected as system clock
     *  - PLLI2S used for I2S clock
     *  - AHB prescaler set to 1
     *  - APB low speed prescaler set to 4
     *  - APB high speed prescaler set to 2
     */
    RCC->CFGR = (0x5 << RCC_CFGR_PPRE1_SHIFT) | (0x4 << RCC_CFGR_PPRE2_SHIFT) | (0x2 << RCC_CFGR_SW_SHIFT);

    /* Wait for PLL to be used as system clock */
    while (((RCC->CFGR & RCC_CFGR_SWS) >> RCC_CFGR_SWS_SHIFT) != 0x2) { }

    /* Enable RTC, set clock to LSE */
    RCC->BDCR |= RCC_BDCR_RTCEN | (0x1 << RCC_BDCR_RTCSEL_SHIFT);

    /* Enable USARTs */
    rcc_APB1_periph_cmd(APB1_UART5, ENABLE);
    rcc_APB1_periph_cmd(APB1_UART4, ENABLE);
    rcc_APB1_periph_cmd(APB1_USART3, ENABLE);
    rcc_APB1_periph_cmd(APB1_USART2, ENABLE);
    rcc_APB2_periph_cmd(APB2_USART6, ENABLE);
    rcc_APB2_periph_cmd(APB2_USART1, ENABLE);

    /* Enable DMAs */
    rcc_AHB1_periph_cmd(AHB1_DMA1, ENABLE);
    rcc_AHB1_periph_cmd(AHB1_DMA2, ENABLE);
}

