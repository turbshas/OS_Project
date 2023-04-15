#ifndef _CPU_REGS_ON_STACK_H
#define _CPU_REGS_ON_STACK_H

#include <cstdint>

/**
 * @brief These registers are automatically pushed to stack by the CPU
 * on entry into an interrupt handler.
 */
class AutomaticallyStackedRegisters
{
    public:
        // This is the lowest address in the stack.
        uint32_t R0;
        uint32_t R1;
        uint32_t R2;
        uint32_t R3;
        uint32_t R12;
        uint32_t LR;
        uint32_t PC;
        // This is the highest address in the stack.
        uint32_t PSR;

        AutomaticallyStackedRegisters();
};

/**
 * @brief Stores registers that do not get automatically saved by the CPU exception handler.
 * The remaining registers are left on the stack, where they are saved.
 * @remark These are saved to the thread instead of the stack to reduce the possibility of a page fault
 * during context switches. If the automatically stacked registers cause one, it results in a a MemManage fault
 * with MSTKERR set. If it is due to us pushing registers on the stack, it causes a fault with DACCVIOL set.
 */
class SavedRegisters
{
    public:
        uint32_t R4;
        uint32_t R5;
        uint32_t R6;
        uint32_t R7;
        uint32_t R8;
        uint32_t R9;
        uint32_t R10;
        uint32_t R11;
        uint32_t SP;

        SavedRegisters();
};

#endif
