#ifndef _SYSCFG_H
#define _SYSCFG_H

#include "chip_common.h"

class SyscfgPeriph {
    uint32_t MEMRM;
    uint32_t PMC;
    uint32_t EXTICR[4];
    uint32_t CMPCR;

    private:
        void set_exti_reg(const int reg, const int shift_amt, const int value) volatile;

    public:
        void set_exti_line(const int line, const int value) volatile;
};

extern volatile SyscfgPeriph *const SYSCFG;

#endif /* _SYSCFG_H */

