#ifndef _RCC_H
#define _RCC_H

#include "chip_common.h"

class RccPeriph {
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

    enum AHB1_periphs { GPIOA = (1u <<  0), GPIOB   = (1u <<  1), GPIOC = (1u <<  2),
                        GPIOD = (1u <<  3), GPIOE   = (1u <<  4), GPIOF = (1u <<  5),
                        GPIOG = (1u <<  6), GPIOH   = (1u <<  7), GPIOI = (1u <<  8),
                        CRC   = (1u << 12), FLITF   = (1u << 15), SRAM1 = (1u << 16),
                        SRAM2 = (1u << 17), BKPSRAM = (1u << 18), DMA1  = (1u << 21),
                        DMA2  = (1u << 22), ETHMAC  = (1u << 25), ETHTX = (1u << 25),
                        ETHRX = (1u << 27), ETHPTP  = (1u << 28), OTGHS = (1u << 29),
                        OTGHSULPI = (1u << 30) };

    enum AHB2_periphs { DCMI = (1u << 0), CRYP  = (1u << 4), HASH = (1u << 5),
                        RNG  = (1u << 6), OTGFS = (1u << 7) };

    enum AHB3_periphs { FSMC = (1u << 0) };

    enum APB1_periphs { TIM2   = (1u <<  0), TIM3   = (1u <<  1), TIM4  = (1u <<  2),
                        TIM5   = (1u <<  3), TIM6   = (1u <<  4), TIM7  = (1u <<  5),
                        TIM12  = (1u <<  6), TIM13  = (1u <<  7), TIM14 = (1u <<  8),
                        WWDG   = (1u << 11), SPI2   = (1u << 14), SPI3  = (1u << 15),
                        USART2 = (1u << 17), USART3 = (1u << 18), UART4 = (1u << 19),
                        UART5  = (1u << 20), I2C1   = (1u << 21), I2C2  = (1u << 22),
                        I2C3   = (1u << 23), CAN1   = (1u << 25), CAN2  = (1u << 26),
                        PWR    = (1u << 28), DAC    = (1u << 29) };

    enum APB2_periphs { TIM1   = (1u <<  0), TIM8   = (1u <<  1), USART1 = (1u <<  4),
                        USART6 = (1u <<  5), ADC    = (1u <<  8), SDIO   = (1u << 11),
                        SPI1   = (1u << 12), SYSCFG = (1u << 14), TIM9   = (1u << 16),
                        TIM10  = (1u << 17), TIM11  = (1u << 18) };

    private:
        RccPeriph() = delete;
        ~RccPeriph() = delete;
        RccPeriph(const RccPeriph&) = delete;
        RccPeriph(RccPeriph&&) = delete;
        void periph_cmd(volatile uint32_t *const reg, const uint32_t periph, const bool state) volatile;
    public:
        /*
         * Periph commands enable/disable each peripheral
         * Low-Power periph commands enable/disable each peripheral in low power mode
         * Reset commands reset each peripheral
         */
        void AHB1_periph_cmd(const enum AHB1_periphs periph, const bool state) volatile;
        void AHB1_LP_periph_cmd(const enum AHB1_periphs periph, const bool state) volatile;
        void AHB1_reset_cmd(const enum AHB1_periphs periph, const bool state) volatile;

        void AHB2_periph_cmd(const enum AHB2_periphs periph, const bool state) volatile;
        void AHB2_LP_periph_cmd(const enum AHB2_periphs periph, const bool state) volatile;
        void AHB2_reset_cmd(const enum AHB2_periphs periph, const bool state) volatile;

        void AHB3_periph_cmd(const enum AHB3_periphs periph, const bool state) volatile;
        void AHB3_LP_periph_cmd(const enum AHB3_periphs periph, const bool state) volatile;
        void AHB3_reset_cmd(const enum AHB3_periphs periph, const bool state) volatile;

        void APB1_periph_cmd(const enum APB1_periphs periph, const bool state) volatile;
        void APB1_LP_periph_cmd(const enum APB1_periphs periph, const bool state) volatile;
        void APB1_reset_cmd(const enum APB1_periphs periph, const bool state) volatile;

        void APB2_periph_cmd(const enum APB2_periphs periph, const bool state) volatile;
        void APB2_LP_periph_cmd(const enum APB2_periphs periph, const bool state) volatile;
        void APB2_reset_cmd(const enum APB2_periphs periph, const bool state) volatile;

        void init() volatile;
};

extern volatile RccPeriph *const RCC;

#endif /* _RCC_H */

