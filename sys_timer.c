#include "sys_timer.h"

#define SYSTIMER_BASE 0xe000e010

#define CSR_COUNTFLAG   (1u << 16)
#define CSR_CLKSOURCE   (1u << 2)
#define CSR_TICKINT     (1u << 1)
#define CSR_ENABLE      (1u << 0)

#define RVR_RELOAD      0x00ffffff
#define CVR_CURRENT     0x00ffffff

#define CALIB_NOREF     (1u << 31)
#define CALIB_SKEW      (1u << 30)
#define CALIB_TENMS     0x00ffffff

struct sys_timer_regs {
    uint32_t CSR;
    uint32_t RVR;
    uint32_t CVR;
    uint32_t CALIB;
};

static volatile struct sys_timer_regs *const SYST = (void *)SYSTIMER_BASE;

/*
register uint32_t r0        asm("r0");
register uint32_t R1        asm("R1");
register uint32_t R2        asm("R2");
register uint32_t R3        asm("R3");
register uint32_t R4        asm("R4");
register uint32_t R5        asm("R5");
register uint32_t R6        asm("R6");
register uint32_t R7        asm("R7");
register uint32_t R8        asm("R8");
register uint32_t R9        asm("R9");
register uint32_t R10       asm("R10");
register uint32_t R11       asm("R11");
register uint32_t R12       asm("R12");
register uint32_t MSP       asm("MSP");
register uint32_t PSP       asm("PSP");
register uint32_t LR        asm("LR");
register uint32_t PC        asm("PC");
register uint32_t PSR       asm("PSR");
register uint32_t PRIMASK   asm("PRIMASK");
register uint32_t FAULTMASK asm("FAULTMASK");
register uint32_t BASEPRI   asm("BASEPRI");
register uint32_t CONTROL   asm("CONTROL");
*/

struct cpu_regs {
    uint32_t R[13];
    uint32_t MSP;
    uint32_t PSP;
    uint32_t LR;
    uint32_t PC;
    uint32_t PSR;
    uint32_t PRIMASK;
    uint32_t FAULTMASK;
    uint32_t BASEPRI;
    uint32_t CONTROL;
};
static struct cpu_regs regs_bkp[2];

static struct cpu_regs *p;
void
SysTick_Handler(void)
{
    asm volatile(
        "LDR    R0, %[bkp_addr]\n\t"
        "STMIA  R0, { R0-R12 }"
        :
        : [bkp_addr] "m" (p)
        : "memory", "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", \
            "r10", "r11", "r12");
    if (p == &regs_bkp[0]) {
        p = &regs_bkp[1];
    } else {
        p = &regs_bkp[0];
    }
}

void
sys_timer_init(void)
{
    p = &regs_bkp[0];
    /*
     * Setup the system timer to tick every 1 ms.
     * System clock is setup for 64 MHz.
     *
     * System timer clock is 64 MHz / 8 = 8 MHz,
     * so set reload value to 7999 so that it ticks
     * every 8000 cycles.
     */
    SYST->RVR &= ~RVR_RELOAD;
    SYST->RVR |= 7999u;

    /* Set clock source to external clock, enable interrupts. */
    SYST->CSR &= ~CSR_CLKSOURCE;
    SYST->CSR |= CSR_TICKINT;
    SYST->CSR |= CSR_ENABLE;
}

