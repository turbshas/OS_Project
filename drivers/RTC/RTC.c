#include "RTC.h"

volatile struct RTC_Regs *const RTC = (void *)RTC_BKP_BASE;

static inline void
disable_write_protection(void)
{
    pwr_disable_bd_write_protection();
    RTC->WPR |= 0xca;
    RTC->WPR |= 0x53;
}

static inline void
enable_write_protection(void)
{
    /* Any invalid key will re-enable write protection */
    RTC->WPR |= 0xff;
    pwr_enable_bd_write_protection();
}

static inline int
enter_init_mode(void)
{
    RTC->ISR |= RTC_ISR_INIT;

    /* Takes 1-2 RTCCLK cycles to actually enter initialization mode */
    unsigned counter = 0;
    while((RTC->ISR & RTC_ISR_INITF) == 0) {
        counter++;
        if (counter >= RTC_INIT_TIMEOUT) {
            return -1;
        }
    }
    return 0;
}

static inline void
exit_init_mode(void)
{
    RTC->ISR &= ~RTC_ISR_INIT;
}

void
RTC_Init(void)
{
    disable_write_protection();
    if (enter_init_mode() < 0) {
        enable_write_protection();
        return;
    }

    /* Set prescaler values for a 1 Hz clock */
    /* Required to do sync first, then async */
    RTC->PRER &= ~(RTC_PRER_ASYNC | RTC_PRER_SYNC);
    RTC->PRER |= 255u & RTC_PRER_SYNC;
    RTC->PRER |= (127u << RTC_PRER_ASYNC_SHIFT) & RTC_PRER_ASYNC;

    /* Set time format to 24-hour */
    RTC->CR &= ~RTC_CR_FMT;

    exit_init_mode();
    enable_write_protection();
}

