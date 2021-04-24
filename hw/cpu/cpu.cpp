#include "cpu.h"
#include "sys_ctl_block.h"
#include "thread.h"

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

bool switchToScheduler;
Thread *schedulerThread;
Thread *runningThread;
CpuRegsOnStack *runningThreadStack;
extern unsigned _INITIAL_STACK_POINTER;

__attribute__((noreturn))
void
threadScheduler(void)
{
    for ( ;; ) {}
}

__attribute__((interrupt, naked, noreturn))
void
PendSV_Handler(void)
{
    // Save registers to stack immediately - regardless of previous thread
    // Save registers R4-R11 of the previously running thread to the stack
    asm volatile (
        "\n\t" "LDR     R0, [%0]"
        "\n\t" "STMDB   R0!, { R4-R11 }"
        :
        : "r" (&runningThreadStack)
        : "r0", "memory");
    
    SYS_CTL->clear_pending_pendsv();

    if (switchToScheduler) {
        // Set the registers on the stack to prepare for the scheduler (privileged mode, main stack pointer, PC)
        const uintptr_t initialStackPointerInt = reinterpret_cast<uintptr_t>(&_INITIAL_STACK_POINTER);
        CpuRegsOnStack *const stackPointer = reinterpret_cast<CpuRegsOnStack *>(initialStackPointerInt - sizeof(CpuRegsOnStack) + offsetof(CpuRegsOnStack, R0));
        stackPointer->PC = reinterpret_cast<uint32_t>(threadScheduler);
        stackPointer->PSR = (1u << 24);
        const uint32_t linkRegisterValue = 0xfffffff9; // Return to thread mode, get state from main stack

        // Set the main stack pointer and return
        asm volatile (
            "\n\t" "MSR     MSP, %0"
            "\n\t" "MOV     LR, %1"
            "\n\t" "BX      LR"
            : : "r" (stackPointer), "r" (linkRegisterValue));
    } else {
        // Need to setup the next threads registers
    }
}

void cpu_init(void) {
    for (int i = 0; i < 10; i++) {}
}

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

#endif
