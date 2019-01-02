#include "stm32_rcc.h"

#define RCC_BASE            (PERIPH_BASE + 0x23800)

volatile struct RCC_Regs *const RCC = (void *)RCC_BASE;

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
#define APB1_UART3      (1u << 18)
#define APB1_UART2      (1u << 17)
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

#define STATE_ON  1
#define STATE_OFF 0
static inline void
set_reg_bit(volatile uint32_t *const reg, const int bit, const int state)
{
    switch (state) {
    case STATE_OFF:
        *reg &= ~bit;
        break;
    case STATE_ON:
        *reg |= bit;
        break;
    default: break;
    }
}

void
rcc_AHB1_set_periph_state(const enum rcc_AHB1_periph periph, const enum rcc_state state)
{
    int bit = 0;
    switch (periph) {
    case RCC_AHB1_OTGHSULPI: bit = AHB1_OTGHSULPI; break;
    case RCC_AHB1_OTGHS:     bit = AHB1_OTGHS;     break;
    case RCC_AHB1_ETHPTP:    bit = AHB1_ETHPTP;    break;
    case RCC_AHB1_ETHRX:     bit = AHB1_ETHRX;     break;
    case RCC_AHB1_ETHTX:     bit = AHB1_ETHTX;     break;
    case RCC_AHB1_ETHMAC:    bit = AHB1_ETHMAC;    break;
    case RCC_AHB1_DMA2:      bit = AHB1_DMA2;      break;
    case RCC_AHB1_DMA1:      bit = AHB1_DMA1;      break;
    case RCC_AHB1_BKPSRAM:   bit = AHB1_BKPSRAM;   break;
    case RCC_AHB1_SRAM2:     bit = AHB1_SRAM2;     break;
    case RCC_AHB1_SRAM1:     bit = AHB1_SRAM1;     break;
    case RCC_AHB1_FLITF:     bit = AHB1_FLITF;     break;
    case RCC_AHB1_CRC:       bit = AHB1_CRC;       break;
    case RCC_AHB1_GPIOI:     bit = AHB1_GPIOI;     break;
    case RCC_AHB1_GPIOH:     bit = AHB1_GPIOH;     break;
    case RCC_AHB1_GPIOG:     bit = AHB1_GPIOG;     break;
    case RCC_AHB1_GPIOF:     bit = AHB1_GPIOF;     break;
    case RCC_AHB1_GPIOE:     bit = AHB1_GPIOE;     break;
    case RCC_AHB1_GPIOD:     bit = AHB1_GPIOD;     break;
    case RCC_AHB1_GPIOC:     bit = AHB1_GPIOC;     break;
    case RCC_AHB1_GPIOB:     bit = AHB1_GPIOB;     break;
    case RCC_AHB1_GPIOA:     bit = AHB1_GPIOA;     break;
    default: break;
    }

    switch (state) {
    case RCC_STATE_RESET:           set_reg_bit(&RCC->AHB1RSTR, bit, STATE_ON);     break;
    case RCC_STATE_CLOCK_DISABLE:   set_reg_bit(&RCC->AHB1ENR, bit, STATE_OFF);     break;
    case RCC_STATE_CLOCK_ENABLE:    set_reg_bit(&RCC->AHB1ENR, bit, STATE_ON);      break;
    case RCC_STATE_CLOCK_LPDISABLE: set_reg_bit(&RCC->AHB1LPENR, bit, STATE_OFF);   break;
    case RCC_STATE_CLOCK_LPENABLE:  set_reg_bit(&RCC->AHB1LPENR, bit, STATE_ON);    break;
    default: break;
    }
}

void
rcc_AHB2_set_periph_state(const enum rcc_AHB2_periph periph, const enum rcc_state state)
{
    int bit = 0;
    switch (periph) {
    case RCC_AHB2_OTGFS:    bit = AHB2_OTGFS; break;
    case RCC_AHB2_RNG:      bit = AHB2_RNG;   break;
    case RCC_AHB2_HASH:     bit = AHB2_HASH;  break;
    case RCC_AHB2_CRYP:     bit = AHB2_CRYP;  break;
    case RCC_AHB2_DCMI:     bit = AHB2_DCMI;  break;
    default: break;
    }

    switch (state) {
    case RCC_STATE_RESET:           set_reg_bit(&RCC->AHB2RSTR, bit, STATE_ON);     break;
    case RCC_STATE_CLOCK_DISABLE:   set_reg_bit(&RCC->AHB2ENR, bit, STATE_OFF);     break;
    case RCC_STATE_CLOCK_ENABLE:    set_reg_bit(&RCC->AHB2ENR, bit, STATE_ON);      break;
    case RCC_STATE_CLOCK_LPDISABLE: set_reg_bit(&RCC->AHB2LPENR, bit, STATE_OFF);   break;
    case RCC_STATE_CLOCK_LPENABLE:  set_reg_bit(&RCC->AHB2LPENR, bit, STATE_ON);    break;
    default: break;
    }
}

void
rcc_AHB3_set_periph_state(const enum rcc_AHB3_periph periph, const enum rcc_state state)
{
    int bit = 0;
    switch (periph) {
    case RCC_AHB3_FSMC:    bit = AHB3_FSMC; break;
    default: break;
    }

    switch (state) {
    case RCC_STATE_RESET:           set_reg_bit(&RCC->AHB3RSTR, bit, STATE_ON);     break;
    case RCC_STATE_CLOCK_DISABLE:   set_reg_bit(&RCC->AHB3ENR, bit, STATE_OFF);     break;
    case RCC_STATE_CLOCK_ENABLE:    set_reg_bit(&RCC->AHB3ENR, bit, STATE_ON);      break;
    case RCC_STATE_CLOCK_LPDISABLE: set_reg_bit(&RCC->AHB3LPENR, bit, STATE_OFF);   break;
    case RCC_STATE_CLOCK_LPENABLE:  set_reg_bit(&RCC->AHB3LPENR, bit, STATE_ON);    break;
    default: break;
    }
}

void
rcc_APB1_set_periph_state(const enum rcc_APB1_periph periph, const enum rcc_state state)
{
    int bit = 0;
    switch (periph) {
    case RCC_APB1_DAC:      bit = APB1_DAC;     break;
    case RCC_APB1_PWR:      bit = APB1_PWR;     break;
    case RCC_APB1_CAN2:     bit = APB1_CAN2;    break;
    case RCC_APB1_CAN1:     bit = APB1_CAN1;    break;
    case RCC_APB1_I2C3:     bit = APB1_I2C3;    break;
    case RCC_APB1_I2C2:     bit = APB1_I2C2;    break;
    case RCC_APB1_I2C1:     bit = APB1_I2C1;    break;
    case RCC_APB1_UART5:    bit = APB1_UART5;   break;
    case RCC_APB1_UART4:    bit = APB1_UART4;   break;
    case RCC_APB1_USART3:    bit = APB1_UART3;   break;
    case RCC_APB1_USART2:    bit = APB1_UART2;   break;
    case RCC_APB1_SPI3:     bit = APB1_SPI3;    break;
    case RCC_APB1_SPI2:     bit = APB1_SPI2;    break;
    case RCC_APB1_WWDG:     bit = APB1_WWDG;    break;
    case RCC_APB1_TIM14:    bit = APB1_TIM14;   break;
    case RCC_APB1_TIM13:    bit = APB1_TIM13;   break;
    case RCC_APB1_TIM12:    bit = APB1_TIM12;   break;
    case RCC_APB1_TIM7:     bit = APB1_TIM7;    break;
    case RCC_APB1_TIM6:     bit = APB1_TIM6;    break;
    case RCC_APB1_TIM5:     bit = APB1_TIM5;    break;
    case RCC_APB1_TIM4:     bit = APB1_TIM4;    break;
    case RCC_APB1_TIM3:     bit = APB1_TIM3;    break;
    case RCC_APB1_TIM2:     bit = APB1_TIM2;    break;
    default: break;
    }

    switch (state) {
    case RCC_STATE_RESET:           set_reg_bit(&RCC->APB1RSTR, bit, STATE_ON);     break;
    case RCC_STATE_CLOCK_DISABLE:   set_reg_bit(&RCC->APB1ENR, bit, STATE_OFF);     break;
    case RCC_STATE_CLOCK_ENABLE:    set_reg_bit(&RCC->APB1ENR, bit, STATE_ON);      break;
    case RCC_STATE_CLOCK_LPDISABLE: set_reg_bit(&RCC->APB1LPENR, bit, STATE_OFF);   break;
    case RCC_STATE_CLOCK_LPENABLE:  set_reg_bit(&RCC->APB1LPENR, bit, STATE_ON);    break;
    default: break;
    }
}

void
rcc_APB2_set_periph_state(const enum rcc_APB2_periph periph, const enum rcc_state state)
{
    int bit = 0;
    switch (periph) {
    case RCC_APB2_TIM11:    bit = APB2_TIM11;   break;
    case RCC_APB2_TIM10:    bit = APB2_TIM10;   break;
    case RCC_APB2_TIM9:     bit = APB2_TIM9;    break;
    case RCC_APB2_SYSCFG:   bit = APB2_SYSCFG;  break;
    case RCC_APB2_SPI1:     bit = APB2_SPI1;    break;
    case RCC_APB2_SDIO:     bit = APB2_SDIO;    break;
    case RCC_APB2_ADC:      bit = APB2_ADC;     break;
    case RCC_APB2_USART6:   bit = APB2_USART6;  break;
    case RCC_APB2_USART1:   bit = APB2_USART1;  break;
    case RCC_APB2_TIM8:     bit = APB2_TIM8;    break;
    case RCC_APB2_TIM1:     bit = APB2_TIM1;    break;
    default: break;
    }

    switch (state) {
    case RCC_STATE_RESET:           set_reg_bit(&RCC->APB2RSTR, bit, STATE_ON);     break;
    case RCC_STATE_CLOCK_DISABLE:   set_reg_bit(&RCC->APB2ENR, bit, STATE_OFF);     break;
    case RCC_STATE_CLOCK_ENABLE:    set_reg_bit(&RCC->APB2ENR, bit, STATE_ON);      break;
    case RCC_STATE_CLOCK_LPDISABLE: set_reg_bit(&RCC->APB2LPENR, bit, STATE_OFF);   break;
    case RCC_STATE_CLOCK_LPENABLE:  set_reg_bit(&RCC->APB2LPENR, bit, STATE_ON);    break;
    default: break;
    }
}

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
    rcc_APB1_set_periph_state(RCC_APB1_UART5, RCC_STATE_CLOCK_ENABLE);
    rcc_APB1_set_periph_state(RCC_APB1_UART4, RCC_STATE_CLOCK_ENABLE);
    rcc_APB1_set_periph_state(RCC_APB1_USART3, RCC_STATE_CLOCK_ENABLE);
    rcc_APB1_set_periph_state(RCC_APB1_USART2, RCC_STATE_CLOCK_ENABLE);
    rcc_APB2_set_periph_state(RCC_APB2_USART6, RCC_STATE_CLOCK_ENABLE);
    rcc_APB2_set_periph_state(RCC_APB2_USART1, RCC_STATE_CLOCK_ENABLE);
    /*RCC->APB1ENR |= RCC_APB1ENR_UART5EN | RCC_APB1ENR_UART4EN | RCC_APB1ENR_USART3EN | RCC_APB1ENR_USART2EN;
    RCC->APB2ENR |= RCC_APB2ENR_USART6EN | RCC_APB2ENR_USART1EN;*/

    /* Enable DMAs */
    RCC->AHB1ENR |= (1u << 22) | (1u << 21);
}

