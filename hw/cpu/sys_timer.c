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

static inline void
disable_sys_tick(void)
{
    SYST->CSR &= ~CSR_TICKINT;
}

static inline void
enable_sys_tick(void)
{
    SYST->CSR |= CSR_TICKINT;
}

struct cpu_regs {
    uint32_t R[13];
    uint32_t LR;
    uint32_t PC;
    uint32_t PSR;
    uint32_t MSP;
    uint32_t PSP;
    uint32_t PRIMASK;
    uint32_t FAULTMASK;
    uint32_t BASEPRI;
    uint32_t CONTROL;
};
/*
 * regs_bkp[0] is for thread_1
 * regs_bkp[1] is for context_switcher
 * regs_bkp[2] is for thread_2
 */
static struct cpu_regs regs_bkp[3];
static struct cpu_regs *p;

static uint32_t t1_stack[64];
void
thread_1(void)
{
    usart_send_string(USART3, "hello ", sizeof("hello "));
    asm("WFI");
}

static uint32_t t2_stack[64];
static void
thread_2(void)
{
    usart_send_string(USART3, "world\n", sizeof("world\n"));
    asm("WFI");
}

__attribute__((naked))
void
SVCall_Handler(void)
{
    register void *val asm("r12");
    val = &p;
    asm volatile (
    /* R12 is pointing to the next thread to run. */
    "LDR    R12, [%0]\n\t"
    /* Load the stack pointer. */
    "LDR    R0, [R12, %1]\n\t"
    "SUB    R0, R0, #0x20\n\t"
    "MSR    MSP, R0\n\t"
    "ISB\n\t"

    /* Load R0 to R11. */
    "LDMIA  R12!, { R0-R11 }\n\t"
    /* Save R0 to R3 to their position in exception stack. */
    "STMIA  SP, { R0-R3 }\n\t"

    /* Load R0 to R3 with R12, LR, PC, PSR. */
    "LDMIA  R12!, { R0-R3 }\n\t"
    /* Save them to the stack. */
    "STR    R0, [SP, #0x10]\n\t"
    "STR    R1, [SP, #0x14]\n\t"
    "STR    R2, [SP, #0x18]\n\t"
    "STR    R3, [SP, #0x1c]\n\t"

    "LDMIA  R12!, { R0-R3 }\n\t"
    "MSR    PSP, R1\n\t"
    "MSR    PRIMASK, R2\n\t"
    "MSR    FAULTMASK, R3\n\t"
    "LDMIA  R12!, { R1-R2 }\n\t"
    "MSR    BASEPRI, R1\n\t"
    "MSR    CONTROL, R2\n\t"
    "BX     LR\n\t"
    :
    : "r" (val), "i" (offsetof(struct cpu_regs, MSP))
    : "memory" );
}

static uint32_t ctx_stack[64];
__attribute__((naked, noreturn))
static void
ctx_switcher(void)
{
    disable_sys_tick();
    if (p == &regs_bkp[0]) {
        p = &regs_bkp[2];
        regs_bkp[2].PC = (uint32_t)thread_2;
    } else {
        p = &regs_bkp[0];
        regs_bkp[0].PC = (uint32_t)thread_1;
    }
    enable_sys_tick();
    asm volatile ("SVC #1");
}

__attribute__((naked, interrupt))
void
SysTick_Handler(void)
{
    register void *val asm("r12");
    val = &p;
    asm volatile (
    /* Save register R0 to R11. */
    "LDR    R12, [%0]\n\t"
    "STMIA  R12!, { R0-R11 }\n\t"
    /* R12 was saved on the stack when we entered the interrupt, get it. */
    "LDR    R0, [SP, #0x10]\n\t"
    /* Save special registers. */
    "LDR    R1, [SP, #0x14]\n\t" /* LR saved on stack. */
    "LDR    R2, [SP, #0x18]\n\t" /* PC saved on stack. */
    "LDR    R3, [SP, #0x1c]\n\t" /* PSR saved on stack. */
    "MRS    R4, MSP\n\t"
     /* MSP stores saved registers, need value before exception triggered. */
    "ADD    R4, R4, #0x20\n\t"
    "MRS    R5, PSP\n\t"
    "MRS    R6, PRIMASK\n\t"
    "MRS    R7, FAULTMASK\n\t"
    "MRS    R8, BASEPRI\n\t"
    "MRS    R9, CONTROL\n\t"
    /* Save registers. */
    "STMIA  R12!, { R0-R9 }\n\t"
    :
    : "r" (val)
    : "memory" );

    val = &regs_bkp[1];

    asm volatile (
    /* R12 is now pointing to regs_bkp[1], load ctx_switcher. */
    /* Load the stack pointer. */
    "LDR    R0, [R12, %1]\n\t"
    "SUB    R0, R0, #0x20\n\t"
    "MSR    MSP, R0\n\t"
    "ISB\n\t"

    /* Load R0 to R11. */
    "LDMIA  R12!, { R0-R11 }\n\t"
    /* Save R0 to R3 to their position in exception stack. */
    "STMIA  SP, { R0-R3 }\n\t"

    /* Load R0 to R3 with R12, LR, PC, PSR. */
    "LDMIA  R12!, { R0-R3 }\n\t"
    /* Save them to the stack. */
    "STR    R0, [SP, #0x10]\n\t"
    "STR    R1, [SP, #0x14]\n\t"
    "STR    R2, [SP, #0x18]\n\t"
    "STR    R3, [SP, #0x1c]\n\t"

    "LDMIA  R12!, { R0-R3 }\n\t"
    "MSR    PSP, R1\n\t"
    "MSR    PRIMASK, R2\n\t"
    "MSR    FAULTMASK, R3\n\t"
    "LDMIA  R12!, { R1-R2 }\n\t"
    "MSR    BASEPRI, R1\n\t"
    "MSR    CONTROL, R2\n\t"
    "BX     LR\n\t"
    :
    : "r" (val), "i" (offsetof(struct cpu_regs, MSP))
    : "memory" );
}

void
sys_timer_init(void)
{
    p = &regs_bkp[0];
    /*
     * Setup the system timer to tick every 8 ms.
     * System clock is setup for 64 MHz.
     *
     * System timer clock is 64 MHz / 8 = 8 MHz,
     * so set reload value to 63999 so that it ticks
     * every 64000 cycles.
     */
    SYST->RVR &= ~RVR_RELOAD;
    SYST->RVR |= 63999u;

    /* Set clock source to external clock, enable interrupts. */
    SYST->CSR &= ~CSR_CLKSOURCE;
    SYST->CSR |= CSR_TICKINT;
    SYST->CSR |= CSR_ENABLE;

    /* Setup registers for testing. */
    regs_bkp[0].PC = (uint32_t)thread_1;
    regs_bkp[0].MSP = (uint32_t)&t1_stack[63];

    regs_bkp[1].PC = (uint32_t)ctx_switcher;
    regs_bkp[1].PSR = 0x01000000;
    regs_bkp[1].MSP = (uint32_t)&ctx_stack[63];

    regs_bkp[2].PC = (uint32_t)thread_2;
    regs_bkp[2].PSR = 0x01000000;
    regs_bkp[2].MSP = (uint32_t)&t2_stack[63];
    /* Everything else 0. */
}

