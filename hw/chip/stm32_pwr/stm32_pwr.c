#include "stm32_pwr.h"

#define PWR_BASE            (PERIPH_BASE + 0x7000)

#define PWR_CR_DBP (1u << 8)

struct PWR_Regs {
    uint32_t CR;
    uint32_t CSR;
};

volatile struct PWR_Regs *const PWR = (void *)PWR_BASE;

void
pwr_disable_bd_write_protection(void)
{
    PWR->CR |= PWR_CR_DBP;
}

void
pwr_enable_bd_write_protection(void)
{
    PWR->CR &= ~PWR_CR_DBP;
}

void
PWR_Init(void)
{
}

