#include "RTC.h"

struct RTC_Regs *const RTC = (void *)RTC_BKP_BASE;

static inline void
Disable_Write_Protection(void)
{
    RTC->WPR |= 0xca;
    RTC->WPR |= 0x53;
}

static inline void
Enable_Write_Protection(void)
{
    /* Any invalid key will re-enable write protection */
    RTC->WPR |= 0xff;
}

static inline int
Enter_Init_Mode(void)
{
    RTC->ISR |= RTC_ISR_INIT;

    /* Takes 1-2 RTCCLK cycles to actually enter initialization mode */
    unsigned counter = 0;
    while((RTC->ISR & RTC_ISR_INITF) == 0) {
        counter++;
        if (counter >= 256) {
            return -1;
        }
    }
    return 0;
}

static inline void
Exit_Init_Mode(void)
{
    RTC->ISR &= ~RTC_ISR_INIT;
}

void
RTC_Init(void)
{
    Disable_Write_Protection();
    if (Enter_Init_Mode() < 0) {
        Enable_Write_Protection();
        return;
    }

    /* Set prescaler values for a 1 Hz clock */
    /* Required to do sync first, then async */
    RTC->PRER &= ~(RTC_PRER_ASYNC | RTC_PRER_SYNC);
    RTC->PRER |= 255u & RTC_PRER_SYNC;
    RTC->PRER |= (127u << RTC_PRER_ASYNC_SHIFT) & RTC_PRER_ASYNC;

    /* Set time format to 24-hour */
    RTC->CR &= ~RTC_CR_FMT;

    Exit_Init_Mode();
    Enable_Write_Protection();
}

