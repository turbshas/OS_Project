#include "stm32_syscfg.h"

#define SYSCFG_BASE (PERIPH_BASE + 0x13800)

#define SYSCFG_MEMMODE_MAIN_FLASH   0x0
#define SYSCFG_MEMMODE_SYS_FLASH    0x1
#define SYSCFG_MEMMODE_FSMC1        0x2
#define SYSCFG_MEMMODE_SRAM         0x3

#define SYSCFG_PMC_MII_RMII_SEL (1u << 23)

#define SYSCFG_CMPCR_READY (1u << 8)
#define SYSCFG_CMPCR_CMP_PD (1u << 0)

volatile SyscfgPeriph *const SYSCFG = reinterpret_cast<volatile SyscfgPeriph *>(SYSCFG_BASE);

void
SyscfgPeriph::set_exti_reg(const int reg, const int shift_amt, const int value) volatile
{
    const uint32_t shifted_val = (value & 0xf) << shift_amt;
    EXTICR[reg] &= ~(0xf << shift_amt);
    EXTICR[reg] |= shifted_val;
}

void
SyscfgPeriph::set_exti_line(const int line, const int value) volatile
{
    const int reg = (line & 0xC) >> 2;
    const int line_in_reg = line & 0x3;
    const int line_bits_shift_amt = line_in_reg << 2;
    set_exti_reg(reg, line_bits_shift_amt, value);
}

