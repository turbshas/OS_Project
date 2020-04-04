#include "stm32_pwr.h"

#define PWR_BASE            (PERIPH_BASE + 0x7000)

#define PWR_CR_DBP (1u << 8)

volatile PwrPeriph *const PWR = reinterpret_cast<volatile PwrPeriph *>(PWR_BASE);

void
PwrPeriph::disable_bd_write_protection() volatile
{
    PWR->CR |= PWR_CR_DBP;
}

void
PwrPeriph::enable_bd_write_protection() volatile
{
    PWR->CR &= ~PWR_CR_DBP;
}

void
pwr_init(void)
{
}

