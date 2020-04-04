#ifndef _PWR_H
#define _PWR_H

#include "chip_common.h"

class PwrPeriph {
    uint32_t CR;
    uint32_t CSR;

    public:
        void disable_bd_write_protection() volatile;
        void enable_bd_write_protection() volatile;
};

extern volatile PwrPeriph *const PWR;

void pwr_init(void);

#endif

