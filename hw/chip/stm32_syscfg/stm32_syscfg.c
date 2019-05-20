#include "stm32_syscfg.h"

#define SYSCFG_BASE (PERIPH_BASE + 0x13800)

#define SYSCFG_MEMMODE_MAIN_FLASH   0x0
#define SYSCFG_MEMMODE_SYS_FLASH    0x1
#define SYSCFG_MEMMODE_FSMC1        0x2
#define SYSCFG_MEMMODE_SRAM         0x3

#define SYSCFG_PMC_MII_RMII_SEL (1u << 23)

#define SYSCFG_EXTIx_0 (0xf << 0)
#define SYSCFG_EXTIx_1 (0xf << 4)
#define SYSCFG_EXTIx_2 (0xf << 8)
#define SYSCFG_EXTIx_3 (0xf << 12)

#define SYSCFG_EXTIx_0_SHIFT 0u
#define SYSCFG_EXTIx_1_SHIFT 4u
#define SYSCFG_EXTIx_2_SHIFT 8u
#define SYSCFG_EXTIx_3_SHIFT 12u

#define SYSCFG_CMPCR_READY (1u << 8)
#define SYSCFG_CMPCR_CMP_PD (1u << 0)

static volatile struct syscfg_regs *const SYSCFG = (void *)SYSCFG_BASE;

static void
syscfg_set_exti_reg(const uint32_t reg, const uint32_t line_bits, const uint32_t shift_amt, const uint32_t value)
{
    uint32_t shifted_val = (value & 0xf) << shift_amt;
    SYSCFG->EXTICR[reg] &= ~line_bits;
    SYSCFG->EXTICR[reg] |= shifted_val;
}

void
syscfg_set_exti_line(const uint32_t line, const uint32_t value)
{
    uint32_t reg = (line & 0xC) >> 2;
    switch (line & 0x3) {
    case 0x0: syscfg_set_exti_reg(reg, SYSCFG_EXTIx_0, SYSCFG_EXTIx_0_SHIFT, value); break;
    case 0x1: syscfg_set_exti_reg(reg, SYSCFG_EXTIx_1, SYSCFG_EXTIx_1_SHIFT, value); break;
    case 0x2: syscfg_set_exti_reg(reg, SYSCFG_EXTIx_2, SYSCFG_EXTIx_2_SHIFT, value); break;
    case 0x3: syscfg_set_exti_reg(reg, SYSCFG_EXTIx_3, SYSCFG_EXTIx_3_SHIFT, value); break;
    default: break;
    }
}

