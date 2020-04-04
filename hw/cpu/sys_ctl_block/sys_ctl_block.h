#ifndef _SYS_CTL_BLOCK_H
#define _SYS_CTL_BLOCK_H

#include <stdio.h>

class SysControlBlock {
    uint32_t ACTLR;
    uint32_t rsvd1;

    /* Sys timer registers */
    uint32_t CSR;
    uint32_t RVR;
    uint32_t CVR;
    uint32_t CALIB;

    uint32_t rsvd2[824];
    uint32_t CPUID;
    uint32_t ICSR;
    uint32_t VTOR;
    uint32_t AIRCR;
    uint32_t SCR;
    uint32_t CCR;
    uint32_t SHPR1;
    uint32_t SHPR2;
    uint32_t SHPR3;
    uint32_t SHCRS;
    uint32_t CFSR;
    uint32_t MMSR;
    uint32_t BFSR;
    uint32_t UFSR;
    uint32_t HFSR;
    uint32_t MMAR;
    uint32_t BFAR;
    uint32_t AFSR;

    public:
        void disable_sys_tick(void) volatile;
        void enable_sys_tick(void) volatile;
        void set_pending_pendsv(void) volatile;
        void clear_pending_pendsv(void) volatile;

        void setup_sys_timer(void) volatile;
};

extern volatile SysControlBlock *const SYS_CTL;

#endif /* _SYS_CTL_BLOCK_H */

