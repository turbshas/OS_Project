#ifndef _SYSCFG_H
#define _SYSCFG_H

#include "chip_common.h"

struct syscfg_regs {
    uint32_t MEMRM;
    uint32_t PMC;
    uint32_t EXTICR[4];
    uint32_t CMPCR;
};

void syscfg_set_exti_line(const uint32_t line, const uint32_t value);

#endif /* _SYSCFG_H */

