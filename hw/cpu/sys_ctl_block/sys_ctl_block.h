#ifndef _SYS_CTL_BLOCK_H
#define _SYS_CTL_BLOCK_H

#include <stdio.h>

#define CSR_COUNTFLAG   (1u << 16)
#define CSR_CLKSOURCE   (1u << 2)
#define CSR_TICKINT     (1u << 1)
#define CSR_ENABLE      (1u << 0)

#define RVR_RELOAD      0x00ffffff
#define CVR_CURRENT     0x00ffffff

#define CALIB_NOREF     (1u << 31)
#define CALIB_SKEW      (1u << 30)
#define CALIB_TENMS     0x00ffffff

#define ICSR_PENDSVSET  (1u << 28)
#define ICSR_PENDSVCLR  (1u << 27)

class SysControlBlock {
    uint32_t ACTLR; // Auxiliary Control
    uint32_t rsvd1;

    /* Sys timer registers */
    uint32_t CSR;   // SysTick Control and Status
    uint32_t RVR;   // SysTick Reload Value
    uint32_t CVR;   // SysTick Current Value
    uint32_t CALIB; // SysTick Calibration Value

    uint32_t rsvd2[824];
    uint32_t CPUID; // CPU ID Base
    uint32_t ICSR;  // Interrupt Control and State
    uint32_t VTOR;  // Vector Table Offset
    uint32_t AIRCR; // Application Interrupt and Reset Control
    uint32_t SCR;   // System Control
    uint32_t CCR;   // Configuration and Control
    uint32_t SHPR1; // System Handler Priority 1
    uint32_t SHPR2; // System Handler Priority 2
    uint32_t SHPR3; // System Handler Priority 3
    uint32_t SHCRS; // System Handler Control and State
    uint32_t CFSR;  // Configurable Fault Status
    uint32_t MMSR;  // MemManage Fault Status
    uint32_t BFSR;  // BusFault Status
    uint32_t UFSR;  // UsageFault Status
    uint32_t HFSR;  // HardFault Status
    uint32_t MMAR;  // MemManage Fault Address
    uint32_t BFAR;  // BusFault Address
    uint32_t AFSR;  // Auxiliary Fault Status

    public:
        void disable_sys_tick(void) volatile { CSR &= ~CSR_TICKINT; };
        void enable_sys_tick(void) volatile { CSR |= CSR_TICKINT; };
        void set_pending_pendsv(void) volatile { ICSR |= ICSR_PENDSVSET; };
        void clear_pending_pendsv(void) volatile { ICSR |= ICSR_PENDSVCLR; };

        void initialize(void) volatile;
};

extern volatile SysControlBlock *const SYS_CTL;

#endif /* _SYS_CTL_BLOCK_H */

