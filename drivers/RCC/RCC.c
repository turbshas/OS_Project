#include "RCC.h"

volatile struct RCC_Regs *const RCC = (void *)RCC_BASE;

void
RCC_Init(void)
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
    RCC->APB1ENR |= RCC_APB1ENR_UART5EN | RCC_APB1ENR_UART4EN | RCC_APB1ENR_USART3EN | RCC_APB1ENR_USART2EN;
    RCC->APB2ENR |= RCC_APB2ENR_USART6EN | RCC_APB2ENR_USART1EN;
}

