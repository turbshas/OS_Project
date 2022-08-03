#include "alloc.h"
#include "drivers.h"
#include "mem_mgr.h"
#include "static_circular_buffer.h"
#include "stm32_rtc.h"

/*
 * Current goal: get a process + thread running in user mode
 * probably just make it idle spin/print out a message every few seconds?
 */
void
ker_main(void)
{
    StaticCircularBuffer<size_t, 8> testBuffer;
    // Test stuff
    usart_driver_init();
    usart_send_string(USART3, "hello world\n", sizeof("hello world\n"));

    struct RTC_datetime dt;
    RTC->get_datetime(&dt);

    mem_mgr_init();
    alloc_init();
    void *p = _malloc(64);
    void *p2 = _malloc(64);
    int *p3 = new int[6];
    _free(p);
    delete[] p3;
    _free(p2);
}
