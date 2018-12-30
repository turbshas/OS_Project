#ifndef _PWR_H
#define _PWR_H

#include "driver_common.h"

#define PWR_BASE            (PERIPH_BASE + 0x7000)

#define PWR_CR_DBP (1u << 8)

struct PWR_Regs {
    uint32_t CR;
    uint32_t CSR;
};

extern volatile struct PWR_Regs *const PWR;

void pwr_disable_bd_write_protection(void);
void pwr_enable_bd_write_protection(void);
void PWR_Init(void);

#endif

