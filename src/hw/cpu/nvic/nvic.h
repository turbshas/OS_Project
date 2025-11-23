#ifndef _NVIC_H
#define _NVIC_H

#include <cstdint>

#define NUM_INTERRUPTS 240
// The following numbers are rounded up to the nearest multiple
#define ROUND_UP(val, divisor) ((((val) - 1) / (divisor)) + 1)
#define NUM_INTERRUPT_REGS ROUND_UP(NUM_INTERRUPTS, sizeof(uint32_t) * 8)
#define NUM_PRIORITY_REGS ROUND_UP(NUM_INTERRUPTS, sizeof(uint32_t))

// This controls the IRQs, not the system handlers.
// For those, use the system control block.
class Nvic {
    // Interrup Set Enable
    uint32_t ISER[NUM_INTERRUPT_REGS];
    // Interrup Clear Enable
    uint32_t ICER[NUM_INTERRUPT_REGS];
    // Interrup Set Pending
    uint32_t ISPR[NUM_INTERRUPT_REGS];
    // Interrup Clear Pending
    uint32_t ICPR[NUM_INTERRUPT_REGS];
    // Interrupt Active Bit
    uint32_t IABR[NUM_INTERRUPT_REGS];
    // Interrupt Priority
    uint32_t IPR[NUM_PRIORITY_REGS];
    // Software Trigger Interrupt
    uint32_t STIR;

    public:
        enum class InterruptNumber : uint8_t {
            // TODO: this should be the positive IRQ numbers only, not the handlers
            // NMI = -14,
            // HardFault = -13,
            // MemManageFault = -12,
            // BusFault = -11,
            // UsageFault = -10,
            // SVCall = -5,
            // DebugMonitor = -4,
            // PendSV = -2,
            // SysTick = -1,
        };

        void enableInterrupt(InterruptNumber interruptNum);
        void disableInterrupt(InterruptNumber interruptNum);
        void setPending(InterruptNumber interruptNum);
        void clearPending(InterruptNumber interruptNum);
        void setPriority(InterruptNumber interruptNum, uint8_t priority);
};

#endif
