#include "PWR.h"

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

