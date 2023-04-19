#ifndef _CPU_REGS_ON_STACK_H
#define _CPU_REGS_ON_STACK_H

#include <cstdint>

#define EXCEPTION_LR_BASE 0xfffffff1
#define EXCEPTION_LR_THREAD_MODE (1u << 3)
#define EXCEPTION_LR_PROCESS_STACK (1u << 2)
#define PSR_THUMB_MODE (1u << 24)

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

        uint32_t GetExceptionReturnLR(const bool threadMode, const bool processStack) volatile
        {
            const uint32_t threadModeBit = threadMode ? EXCEPTION_LR_THREAD_MODE : 0;
            const uint32_t processStackBit = processStack || !threadMode ? EXCEPTION_LR_PROCESS_STACK : 0;
            return EXCEPTION_LR_BASE | threadModeBit | processStackBit;
        };
        void SetPC(void* firstInstruction) volatile { PC = reinterpret_cast<uint32_t>(firstInstruction); };
        void SetThumbMode() volatile { PSR = PSR | PSR_THUMB_MODE; };
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
        uint32_t ExceptionLR;

        SavedRegisters();

        uintptr_t GetLoadMultipleStartAddress() const
        {
            const uintptr_t lastRegisterAddr = reinterpret_cast<uintptr_t>(&R11);
            // We use Decrement-Before for LDM instruction, so need to offset by one register-size.
            return lastRegisterAddr + sizeof(uint32_t);
        };
        bool IsFromProcessStack() const
        {
            return ExceptionLR & EXCEPTION_LR_PROCESS_STACK;
        };

        void SetStackPointer(const uintptr_t stackAddress)
        {
            // Stack is full-descending, so reduce stack pointer by the size of the registers that should be there. Plus an extra 4 bytes for safety.
            const uintptr_t stackedRegistersAddress = stackAddress - (sizeof(AutomaticallyStackedRegisters) + sizeof(uint32_t));
            SP = stackedRegistersAddress;
        };
        void SetExceptionLR(const bool threadMode, const bool processStack)
        {
            const uint32_t threadModeBit = threadMode ? EXCEPTION_LR_THREAD_MODE : 0;
            const uint32_t processStackBit = processStack || !threadMode ? EXCEPTION_LR_PROCESS_STACK : 0;
            ExceptionLR = EXCEPTION_LR_BASE | threadModeBit | processStackBit;
        };
};

#endif
