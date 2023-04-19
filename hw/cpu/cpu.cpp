#include "cpu.h"
#include "api_request.hpp"
#include "kernel_api.hpp"
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

Thread* schedulerThread;
Thread* volatile runningThread;
Thread* volatile otherThread; // temp
SavedRegisters* volatile runningThreadSavedRegisters;
extern unsigned _INITIAL_STACK_POINTER;

/* Save registers to stack immediately after interrupt - regardless of previous thread.
    1) Save registers R4-R11 of the previously running thread to the thread
    2) Since this is Increment After, indexing register should be pointing to address of SP in SavedRegisters.
    3) Get stack value.
    4) Bit 2 of LR says which stack is being used
    5) Save stack value.
    6) Save LR.
    - Always Inline is important - we don't want the compile to generate a call here
      since that would clobber LR (which we need to save).
    - This is really only needed for SysTick and SVC since we'll need to save the previous thread's registers to its thread object.
      All other interrupt handlers should use the compiler-generated prologue and epilogue to ensure registers are saved.
*/
__attribute__((always_inline)) static inline void
SAVE_REGISTERS_AFTER_INTERRUPT(void)
{
    register uint32_t r4 asm("r4");
    register uint32_t r5 asm("r5");
    register uint32_t r6 asm("r6");
    register uint32_t r7 asm("r7");
    register uint32_t r8 asm("r8");
    register uint32_t r9 asm("r9");
    register uint32_t r10 asm("r10");
    register uint32_t r11 asm("r11");
    asm volatile(
        "STMIA  %[savedRegsAddr]!, { %[r4], %[r5], %[r6], %[r7], %[r8], %[r9], %[r10], %[r11] }\n\t"
        "TST    LR, %[lrStackBit]\n\t"
        "ITE    EQ\n\t"
        "MRSEQ  R1, MSP\n\t"
        "MRSNE  R1, PSP\n\t"
        "STR    R1, [%[savedRegsAddr]], %[regSize]\n\t"
        "STR    LR, [%[savedRegsAddr]]\n\t"
        :
        : [savedRegsAddr] "r"(runningThreadSavedRegisters),
          [regSize] "i"(sizeof(uint32_t)),
          [lrStackBit] "i"(EXCEPTION_LR_PROCESS_STACK),
          [r4] "r"(r4),
          [r5] "r"(r5),
          [r6] "r"(r6),
          [r7] "r"(r7),
          [r8] "r"(r8),
          [r9] "r"(r9),
          [r10] "r"(r10),
          [r11] "r"(r11)
        : "r1", "memory", "cc");
}

static uint32_t
GetLastStackValue(void)
{
    uint32_t stackPointer;
    asm volatile(
        "TST    LR, %[lrStackBit]\n\t"
        "ITE    EQ\n\t"
        "MRSEQ  %[sp], MSP\n\t"
        "MRSNE  %[sp], PSP\n\t"
        : [sp] "=rm"(stackPointer)
        : [lrStackBit] "i"(EXCEPTION_LR_PROCESS_STACK)
        : "cc");
    return stackPointer;
}

__attribute__((naked, noreturn)) void
threadScheduler(void)
{
    Thread* temp = runningThread;
    runningThread = otherThread;
    otherThread = temp;
    runningThreadSavedRegisters = const_cast<SavedRegisters*>(runningThread->GetSavedRegisters());
    SYS_CTL->set_pending_pendsv();
    for (;;) {}
}

__attribute__((interrupt, noreturn)) void
PendSV_Handler(void)
{
    // No need to save kernel thread registers - they'll be reset on entry to the kernel anyway.
    SYS_CTL->clear_pending_pendsv();
    const auto savedRegs = runningThread->GetSavedRegisters();

    /*
        1) Set saved registers (R4-R11).
        2) Set LR.
        3) Determine which stack will be used.
        4) Set SP.
        5) Return from interrupt.
    */
    asm volatile(
        "LDMDB  %[savedRegsAddr], { R4-R11 }\n\t"
        "LDR    LR, [%[lr]]\n\t"
        "TST    LR, %[lrStackBit]\n\t"
        "ITE    EQ\n\t"
        "MSREQ  MSP, %[sp]\n\t"
        "MSRNE  PSP, %[sp]\n\t"
        "BX     LR\n\t"
        :
        : [savedRegsAddr] "r"(savedRegs->GetLoadMultipleStartAddress()),
          [sp] "r"(savedRegs->SP),
          [lr] "r"(&savedRegs->ExceptionLR),
          [lrStackBit] "i"(EXCEPTION_LR_PROCESS_STACK)
        : "lr", "memory", "cc"); // Are regiser clobbers even necessary here? This function is marked as naked

    while (true) {}
}

// These should only be used for SysTick
static volatile uintptr_t stackPointerInt;
static volatile uintptr_t stackedRegistersStartAddress;
static volatile AutomaticallyStackedRegisters* volatile stackedRegs;
static volatile uint32_t linkRegisterValue;

// This is naked because we need to save registers before the compiler uses them.
// Without this, it's attempting to (push to stack first, then) use R7 which we need to save first.
__attribute__((interrupt, noreturn, naked)) void
SysTick_Handler(void)
{
    SAVE_REGISTERS_AFTER_INTERRUPT();
    SYS_CTL->clear_pending_systick();

    // Set the registers on the stack to prepare for the scheduler (privileged mode, main stack pointer, PC).
    stackPointerInt = reinterpret_cast<uintptr_t>(&_INITIAL_STACK_POINTER);
    // Stack is full-descending, so reduce stack pointer by the size of the registers that should be there. Plus an extra 4 bytes for safety.
    stackedRegistersStartAddress = stackPointerInt - (sizeof(AutomaticallyStackedRegisters) + sizeof(uint32_t));
    stackedRegs = reinterpret_cast<volatile AutomaticallyStackedRegisters*>(stackedRegistersStartAddress);

    stackedRegs->SetPC(reinterpret_cast<void*>(threadScheduler));
    stackedRegs->SetThumbMode();
    // Return to thread mode, get state from main stack.
    linkRegisterValue = stackedRegs->GetExceptionReturnLR(true, false);

    // Set the main stack pointer and return
    asm volatile(
        "MSR     MSP, %[sp]\n\t"
        "MOV     LR, %[lr]\n\t"
        "BX      LR\n\t"
        :
        : [sp] "r"(stackedRegs),
          [lr] "r"(linkRegisterValue));

    while (true) {}
}

__attribute__((interrupt)) void
SVC_Handler(void)
{
    const uint32_t stackPointer = GetLastStackValue();
    const AutomaticallyStackedRegisters* const stackedRegs = reinterpret_cast<const AutomaticallyStackedRegisters*>(stackPointer);
    const os::api::ApiRequest apiRequest{stackedRegs->R0, stackedRegs->R1, stackedRegs->R3};
    os::api::kernelApi.ProcessRequest(apiRequest);
}

void
cpu_init(void)
{
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
