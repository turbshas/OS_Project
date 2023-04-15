#ifndef _CPU_H
#define _CPU_H

#include "savedRegisters.hpp"
#include <stdio.h>

#define NUM_CPUS 1u

class Cpu
{
    public:
        void collectCpuInfo();
        void dispatchThread(SavedRegisters& regs);

    private:
};

#endif /* _CPU_H */
