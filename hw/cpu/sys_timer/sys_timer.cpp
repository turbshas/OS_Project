#include "sys_timer.h"
#include "sys_ctl_block.h"

struct cpu_regs_on_stack
{
        uint32_t R4_11[8];
        uint32_t R0_3[4];
        uint32_t R12;
        uint32_t LR;
        uint32_t PC;
        uint32_t PSR;
};
uint32_t main_stack[64];
static uint32_t ctx_stack[64];
static uint32_t t1_stack[64];
static uint32_t t2_stack[64];
/*
 * thread_sp[0] is for main program
 * thread_sp[1] is for context_switcher
 * thread_sp[2] is for thread_1
 * thread_sp[3] is for thread_2
 */
static struct cpu_regs_on_stack* thread_sp[4];
static void* active_stack;
static void* next_stack;

void
thread_1(void)
{
    usart_send_string(USART3, "hello ", sizeof("hello "));
    asm("WFI");
}

static void
thread_2(void)
{
    usart_send_string(USART3, "world\n", sizeof("world\n"));
    asm("WFI");
}

__attribute__((interrupt, naked, noreturn)) void
old_PendSV_Handler(void)
{
    SYS_CTL->clear_pending_pendsv();

    register void* val asm("r1");
    val = &active_stack;

    asm volatile(
        /* Load the stack pointer. SP has to be 4-byte aligned so we'll need to clear bits 1:0. */
        // Also, since this is a test setup - bit 1 and 0 of the stack pointer being used contain nPRIV and SPSEL, respectively.
        "LDR    %0, [%0]\n\t"
        "LDR    R0, [%0]\n\t"

        /* Restore proper privilege. */
        "MRS    R2, CONTROL\n\t"
        "TST    R0, #0x2\n\t"
        "ITE    EQ\n\t"
        "BICEQ  R2, R2, #0x1\n\t" /* Clear bit 0 (set unprivileged). */
        "ORRNE  R2, R2, #0x1\n\t" /* Set bit 0 (set privileged). */
        "MSR    CONTROL, R2\n\t"
        "BIC    R0, R0, #0x2\n\t" /* Clear bit 1 so we can use R0 as SP. */

        /* Restore stack pointer. */
        "TST    R0, #0x1\n\t"
        "BIC    R0, R0, #0x1\n\t" /* Clear bit 0 so we can use R0 as SP. */
        /* Restore R4-R11. */
        "LDMIA  R0!, { R4-R11 }\n\t"

        "ITTEE  EQ\n\t"
        "MSREQ  PSP, R0\n\t"
        "ORREQ  LR, LR, #0x4\n\t" /* Set bit 2 in LR so PSP is used on return. */
        "MSRNE  MSP, R0\n\t"
        "BICNE  LR, LR, #0x4\n\t" /* Clear bit 2 in LR so MSP is used on return. */
        "ISB\n\t"

        "BX     LR\n\t"
        :
        : "r"(val)
        : "memory");
}

__attribute__((naked, noreturn)) static void
ctx_switcher(void)
{
    asm(
        "ADD    SP, SP, %0"
        :
        : "i"(sizeof(struct cpu_regs_on_stack)));

    SYS_CTL->disable_sys_tick();
    if (active_stack != &thread_sp[3])
    {
        active_stack = &thread_sp[3];
        thread_sp[3]->PC = (uint32_t)thread_2;
    }
    else
    {
        active_stack = &thread_sp[2];
        thread_sp[2]->PC = (uint32_t)thread_1;
    }
    SYS_CTL->enable_sys_tick();
    SYS_CTL->set_pending_pendsv();
}

__attribute__((naked, interrupt)) static void
old_SysTick_Handler(void)
{
    register void* val asm("r1");
    val = &active_stack;
    asm volatile(
        /* Get stack value. */
        "TST    LR, #0x4\n\t" /* Bit 3 of LR says which stack is being used */
        "ITE    EQ\n\t"
        "MRSEQ  R0, MSP\n\t"
        "MRSNE  R0, PSP\n\t"
        /* Save register R4 to R11 to stack. */
        "STMDB  R0!, { R4-R11 }\n\t" /* STM Decrement Before/Full Descending */
        /* Figure out privileges, stack used. */
        /* Privileges and stack used will be stored in the lower 2 bits of the stack pointer as it is 4-byte aligned anyway */
        "MRS    R2, CONTROL\n\t"
        "TST    R2, #0x1\n\t" /* Bit 0 of Control register stores whether priv/unpriv */
        "IT     NE\n\t"
        "ORRNE  R0, R0, #0x2\n\t" /* Priv/unpriv */
        "TST    LR, #0x4\n\t"
        "IT     EQ\n\t"
        "ORREQ  R0, R0, #0x1\n\t" /* MSP/PSP */
        /* Save stack pointer to array. */
        "LDR    %0, [%0]\n\t"
        "STR    R0, [%0]\n\t"
        :
        : "r"(val)
        : "memory");

    val = &thread_sp[1];

    asm volatile(
        /* Load the stack pointer. */
        "LDR    R0, [%0]\n\t"

        /* Restore proper privilege. */
        "MRS    R2, CONTROL\n\t"
        "TST    R0, #0x2\n\t"
        "ITE    EQ\n\t"
        "BICEQ  R2, R2, #0x1\n\t" /* Clear bit 0 (set unprivileged). */
        "ORRNE  R2, R2, #0x1\n\t" /* Set bit 0 (set privileged). */
        "MSR    CONTROL, R2\n\t"
        "BIC    R0, R0, #0x2\n\t" /* Clear bit 1 so we can use R0 as SP. */

        /* Restore stack pointer. */
        "TST    R0, #0x1\n\t"
        "BIC    R0, R0, #0x1\n\t" /* Clear bit 0 so we can use R0 as SP. */
        /* Restore R4-R11. */
        "LDMIA  R0!, { R4-R11 }\n\t"

        "ITTEE  EQ\n\t"
        "MSREQ  PSP, R0\n\t"
        "ORREQ  LR, LR, #0x4\n\t" /* Set bit 2 in LR so PSP is used on return. */
        "MSRNE  MSP, R0\n\t"
        "BICNE  LR, LR, #0x4\n\t" /* Clear bit 2 in LR so MSP is used on return. */
        "ISB\n\t"

        "BX     LR\n\t"
        :
        : "r"(val)
        : "memory");
}

uint32_t numSystemTicks;

__attribute__((interrupt)) void
second_old_SysTick_Handler(void)
{
    numSystemTicks++;
    SYS_CTL->set_pending_pendsv();
}

void
sys_timer_init(void)
{
    active_stack = (void*)&thread_sp[0];
    next_stack = (void*)&thread_sp[1];
    /*
     * bit 1 is used for privileged(1)/unprivileged(0).
     * bit 0 is used for MSP(1)/PSP(0)
     */
    thread_sp[0] = reinterpret_cast<struct cpu_regs_on_stack*>((uint32_t)&main_stack[63] | 0x3);

    /* Setup registers for testing. */
    struct cpu_regs_on_stack* p;
    p = reinterpret_cast<struct cpu_regs_on_stack*>(&ctx_stack[63] - (sizeof(struct cpu_regs_on_stack) / 4));
    p->PC = (uint32_t)ctx_switcher;
    p->PSR = 0x01000000;
    thread_sp[1] = reinterpret_cast<struct cpu_regs_on_stack*>((uint32_t)p | 0x2);

    p = reinterpret_cast<struct cpu_regs_on_stack*>(&t1_stack[63] - (sizeof(struct cpu_regs_on_stack) / 4));
    p->PC = (uint32_t)thread_1;
    p->PSR = 0x01000000;
    thread_sp[2] = reinterpret_cast<struct cpu_regs_on_stack*>((uint32_t)p | 0x0);

    p = reinterpret_cast<struct cpu_regs_on_stack*>(&t2_stack[63] - (sizeof(struct cpu_regs_on_stack) / 4));
    p->PC = (uint32_t)thread_2;
    p->PSR = 0x01000000;
    thread_sp[3] = reinterpret_cast<struct cpu_regs_on_stack*>((uint32_t)p | 0x0);
    /* Everything else 0. */

    SYS_CTL->initialize();
}
