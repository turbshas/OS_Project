#ifndef _PWR_H
#define _PWR_H

#include "chip_common.h"

class PwrPeriph {
    uint32_t CR;
    uint32_t CSR;

    private:
        PwrPeriph() = delete;
        ~PwrPeriph() = delete;
        PwrPeriph(const PwrPeriph&) = delete;
        PwrPeriph(PwrPeriph&&) = delete;

    public:
        void disable_bd_write_protection() volatile;
        void enable_bd_write_protection() volatile;
};

extern volatile PwrPeriph *const PWR;

void pwr_init(void);

#endif

