#ifndef _CPU_H
#define _CPU_H

#include <stdio.h>

#include "cpuRegsOnStack.h"

#define NUM_CPUS 1u

class Cpu {
    public:
        void collectCpuInfo();
        void dispatchThread(CpuRegsOnStack& regs);

    private:
};

#endif /* _CPU_H */

