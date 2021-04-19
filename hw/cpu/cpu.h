#ifndef _CPU_H
#define _CPU_H

#include <stdio.h>

#define NUM_CPUS 1u

struct CpuRegsOnStack {
    uint32_t R4_11[8];
    uint32_t R0_3[4];
    uint32_t R12;
    uint32_t LR;
    uint32_t PC;
    uint32_t PSR;
};

class Cpu {
    public:
        void collectCpuInfo();
        void dispatchThread(CpuRegsOnStack& regs);

    private:
};

#endif /* _CPU_H */

