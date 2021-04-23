#ifndef _CPU_REGS_ON_STACK_H
#define _CPU_REGS_ON_STACK_H

#include <cstdint>

struct CpuRegsOnStack {
    /* System tick/supervisor call handlers will places these on the stack */
    uint32_t R4;
    uint32_t R5;
    uint32_t R6;
    uint32_t R7;
    uint32_t R8;
    uint32_t R9;
    uint32_t R10;
    uint32_t R11;

    /* These are automatically placed on the stack by the CPU */
    uint32_t R0;
    uint32_t R1;
    uint32_t R2;
    uint32_t R3;
    uint32_t R12;
    uint32_t LR;
    uint32_t PC;
    uint32_t PSR;

    CpuRegsOnStack();
};

#endif
