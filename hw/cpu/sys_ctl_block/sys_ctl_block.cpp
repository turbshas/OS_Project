#include "sys_ctl_block.h"

#define SYS_CTL_BLOCK_BASE 0xe000e008

#define SHPR3_SYSTICK 0xff000000
#define SHPR3_SYSTICK_SHIFT 24u

#define SHPR3_PENDSV 0xff0000
#define SHPR3_PENDSV_SHIFT 16u

volatile SysControlBlock* const SYS_CTL = reinterpret_cast<volatile SysControlBlock*>(SYS_CTL_BLOCK_BASE);

void
SysControlBlock::initialize(void) volatile
{
    /*
     * Setup the system timer to tick every 8 ms.
     * System clock is setup for 64 MHz.
     *
     * System timer clock is 64 MHz / 8 = 8 MHz,
     * so set reload value to 63999 so that it ticks
     * every 64000 cycles.
     */
    RVR = RVR & ~RVR_RELOAD;
    RVR = RVR | 63999u;

    /* Set clock source to external clock. */
    CSR = CSR & ~CSR_CLKSOURCE;
    CSR = CSR | CSR_TICKINT;

    /* Set PendSV priority to a low amount - should be the last interrupt to run */
    const uint32_t currentSHPR3 = SHPR3;
    const uint32_t pendSvPriority = (255u << SHPR3_PENDSV_SHIFT) & SHPR3_PENDSV;
    const uint32_t sysTickPriority = (254u << SHPR3_SYSTICK_SHIFT) & SHPR3_SYSTICK;
    SHPR3 = currentSHPR3 | pendSvPriority | sysTickPriority;
}
