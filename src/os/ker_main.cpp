#include "alloc.h"
#include "drivers.h"
#include "kernel_api.hpp"
#include "mem_mgr.h"
#include "proc_mgr.h"
#include "savedRegisters.hpp"
#include "static_circular_buffer.h"
#include "stm32_rtc.h"
#include "sys_ctl_block.h"

using namespace os::utils::static_buffer;

static MemRegion
AllocateMem(const size_t numBytes)
{
    return memoryManager.Allocate(numBytes);
}

static void
OnAllocateComplete(const MemRegion& memRegion)
{
    processManager.GetKernelProcess()->AddMemRegion(memRegion);
}

// TEST HERE:
static const char oneText[] = "1";
static const char twoText[] = "2";
static const char threeText[] = "3";
static const char fourText[] = "4";
static const char fiveText[] = "5";
static const char helloText[] = "hello ";
static void
thread1(void)
{
    while (true)
    {
        usart_send_string(USART1, helloText, sizeof(helloText));
        usart_send_string(USART1, oneText, sizeof(oneText));
        asm("WFI");
        usart_send_string(USART1, helloText, sizeof(helloText));
        usart_send_string(USART1, twoText, sizeof(twoText));
        asm("WFI");
        usart_send_string(USART1, helloText, sizeof(helloText));
        usart_send_string(USART1, threeText, sizeof(threeText));
        asm("WFI");
        usart_send_string(USART1, helloText, sizeof(helloText));
        usart_send_string(USART1, fourText, sizeof(fourText));
        asm("WFI");
        usart_send_string(USART1, helloText, sizeof(helloText));
        usart_send_string(USART1, fiveText, sizeof(fiveText));
        asm("WFI");
    }
}

static const char worldText[] = "world\n";
static void
thread2(void)
{
    while (true)
    {
        usart_send_string(USART1, worldText, sizeof(worldText));
        usart_send_string(USART1, oneText, sizeof(oneText));
        asm("WFI");
        usart_send_string(USART1, worldText, sizeof(worldText));
        usart_send_string(USART1, twoText, sizeof(twoText));
        asm("WFI");
        usart_send_string(USART1, worldText, sizeof(worldText));
        usart_send_string(USART1, threeText, sizeof(threeText));
        asm("WFI");
        usart_send_string(USART1, worldText, sizeof(worldText));
        usart_send_string(USART1, fourText, sizeof(fourText));
        asm("WFI");
        usart_send_string(USART1, worldText, sizeof(worldText));
        usart_send_string(USART1, fiveText, sizeof(fiveText));
        asm("WFI");
    }
}

extern Thread* schedulerThread;
extern Thread* volatile runningThread;
extern Thread* volatile otherThread;
extern SavedRegisters* volatile runningThreadSavedRegisters;

extern void
threadScheduler(void);

static void
startExecution(Thread* scheduler, Thread* thread1, Thread* thread2)
{
    schedulerThread = scheduler;
    runningThread = thread1;
    otherThread = thread2;
    runningThreadSavedRegisters = const_cast<SavedRegisters*>(thread1->GetSavedRegisters());
}

static void
disableInterrupts(void)
{
    asm volatile(
        "MRS  R0, FAULTMASK\n\t"
        "ORR  R0, R0, #0x1\n\t"
        "MSR  FAULTMASK, R0\n\t"
        :
        :
        : "r0");
}

static void
enableInterrupts(void)
{
    SYS_CTL->clear_pending_pendsv();
    SYS_CTL->clear_pending_systick();
    asm volatile(
        "MRS  R0, FAULTMASK\n\t"
        "AND  R0, R0, %0\n\t"
        "MSR  FAULTMASK, R0\n\t"
        :
        : "i"(~0x1u)
        : "r0");
}
// TEST END

/*
 * Current goal: get a process + thread running in user mode
 * probably just make it idle spin/print out a message every few seconds?
 */
void
ker_main(void)
{
    /* TODO:
        - Create Process for kernel
          - this will hold the kernel thread and dynamically allocated memory
          - any switch into the kernel (service requests, interrupts, etc.) will have the same process but code
            execution will probably be in the same location - some handler of some kind?
              - for service requests, the service request handler
                - service request handler will need to accept parameters via register, can peek at stacked registers in previous thread for that
              - for interrupts, something else?
        - Requires setting up MPU to block access to any allocated kernel memory + static memory (code + static data)
        - Process manager needs reference to memory manager
        - Threads/Processes need to be understandable outside of process manager?
          - other places will probably need to access them somehow, like context switching
        - Create process for user
          - Will context switch out of kernel process and into user process
          - Will need to allocate stack storage
    */
    disableInterrupts();
    usart_driver_init();
    usart_send_string(USART1, "hello world\n", sizeof("hello world\n"));

    memoryManager.Initialize();
    kernelApi.ApiEntry = threadScheduler; // temp
    processManager.Initialize(memoryManager, kernelApi);
    alloc_init(AllocateMem, OnAllocateComplete);
    auto process1 = processManager.CreateProcess(thread1);
    auto process2 = processManager.CreateProcess(thread2);
    startExecution(processManager.GetKernelProcess()->GetMainThread(), process1->GetMainThread(), process2->GetMainThread());
    enableInterrupts();
    SYS_CTL->enable_sys_tick();
    SYS_CTL->set_pending_pendsv();
    for (;;) {}
}
