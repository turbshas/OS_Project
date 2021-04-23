#include "cpu.h"

/* SVC Interrupt used for service calls - goes directly to a function that handles requests to make OS calls
 * PendSV used for context switching - from OS back to user process I guess?
 * SysTick used for time slicing - loads the OS thread then chooses which thread to run next
 * 
 * PendSV needs to be set to lowest priority in the system - prevents a context switch from occurring during ISR handlers
 * Option: SysTick sets the thread scheduler to run next, then sets a pending PendSV request to do the context switch when all other ISRs have finished
 *   -> probably this option
 * 
 * Need to handle tail chaining and preemption:
 * - SysTick occuring during a PendSV - should be fine, SysTick tail chains back into the thread scheduler
 * - PendSV during SysTick - shouldn't happen (PendSV should only be caused by OS)
 * - SysTick during SVC - thread scheduler is going to have to check if the running thread was the OS thread
 *    -> return early if it was and let the SVC call complete
 *       (bonus: complete the call but schedule a new thread to run instead of returning to the caller?)
 * - SVC during PendSV, PendSV during SVC, and SVC during SysTick shouldn't happen
 * 
 */

#if 0

static Cpu hw_cpus[NUM_CPUS];

void
Cpu::collectCpuInfo()
{
}

void
Cpu::dispatchThread(Thread& thread)
{
}

__attribute__((naked, interrupt))
void
SysTick_Handler(void)
{
    register void *val asm("r1");
    val = &active_stack;
    asm volatile (
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
    : "r" (val)
    : "memory" );

    val = &thread_sp[1];

    asm volatile (
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
    : "r" (val)
    : "memory" );
}

#endif
