#include "alloc.h"
#include "drivers.h"
#include "mem_mgr.h"
#include "proc_mgr.h"
#include "static_circular_buffer.h"
#include "stm32_rtc.h"

using namespace os::utils::static_buffer;

static const void*
AllocateMem(const size_t numBytes)
{
    // const void* addr = memoryManager.AllocatePages(processManager.GetKernelProcess(), numBytes);
    const void* addr = nullptr;
    return addr;
}

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
    StaticCircularBuffer<size_t, 8> testBuffer;
    // Test stuff
    usart_driver_init();
    usart_send_string(USART3, "hello world\n", sizeof("hello world\n"));

    struct RTC_datetime dt;
    RTC->get_datetime(&dt);

    memoryManager.Initialize();
    alloc_init(AllocateMem);
    void* p = _malloc(64);
    void* p2 = _malloc(64);
    int* p3 = new int[6];
    _free(p);
    delete[] p3;
    _free(p2);
}
