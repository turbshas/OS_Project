#include "sys_ctl_block.h"

#define SYS_CTL_BLOCK_BASE 0xe000e008

#define ICSR_PENDSVSET  (1u << 28)
#define ICSR_PENDSVCLR  (1u << 27)

volatile SysControlBlock *const SYS_CTL = reinterpret_cast<volatile SysControlBlock *>(SYS_CTL_BLOCK_BASE);

inline void
SysControlBlock::disable_sys_tick(void) volatile
{
    SYST->CSR &= ~CSR_TICKINT;
}

inline void
SysControlBlock::enable_sys_tick(void) volatile
{
    SYST->CSR |= CSR_TICKINT;
}

inline void
SysControlBlock::set_pending_pendsv(void) volatile
{
    ICSR |= ICSR_PENDSVSET;
}

inline void
SysControlBlock::clear_pending_pendsv(void) volatile
{
    ICSR |= ICSR_PENDSVCLR;
}

void
SysControlBlock::setup_sys_timer(void) volatile
{
    /*
     * Setup the system timer to tick every 8 ms.
     * System clock is setup for 64 MHz.
     *
     * System timer clock is 64 MHz / 8 = 8 MHz,
     * so set reload value to 63999 so that it ticks
     * every 64000 cycles.
     */
    RVR &= ~RVR_RELOAD;
    RVR |= 63999u;

    /* Set clock source to external clock, enable interrupts. */
    CSR &= ~CSR_CLKSOURCE;
    CSR |= CSR_TICKINT;
    CSR |= CSR_ENABLE;
}

