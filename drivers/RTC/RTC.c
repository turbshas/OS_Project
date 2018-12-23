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

static inline void
disable_wut(void)
{
    /* Disable WUT */
    RTC->CR &= ~RTC_CR_WUTE;

    /* Wait for write flag to go high */
    while ((RTC->ISR & RTC_ISR_WUTWF) == 0) { }
}

static inline void
enable_wut(void)
{
    RTC->CR |= RTC_CR_WUTE;
}

static inline int
wait_for_synchro(void)
{
    uint32_t counter = 0;

    while ((RTC->ISR & RTC_ISR_RSF) == 0) {
        if (counter == RTC_SYNCHRO_TIMEOUT) {
            return -1;
        }
        counter++;
    }

    return 0;
}

int
RTC_get_datetime(struct RTC_datetime *const datetime)
{
    if ((RTC->ISR & RTC_ISR_RSF) == 0) {
        if (wait_for_synchro() < 0) {
            return -1;
        }
    }

    const uint32_t time_reg = RTC->TR;
    const uint32_t date_reg = RTC->DR;

    /* Year stored in time register is relative to the year 2000 */
    datetime->year    = RTC_GET_BCD(date_reg, RTC_DR_Y) + 2000;
    datetime->month   = RTC_GET_BCD(date_reg, RTC_DR_M);
    datetime->day     = RTC_GET_BCD(date_reg, RTC_DR_D);
    datetime->day_of_week = RTC_GET_VAL(date_reg, RTC_DR_WDU);

    datetime->hours   = RTC_GET_BCD(time_reg, RTC_TR_H);
    datetime->minutes = RTC_GET_BCD(time_reg, RTC_TR_MN);
    datetime->seconds = RTC_GET_BCD(time_reg, RTC_TR_S);

    return 0;
}

int
RTC_set_datetime(const struct RTC_datetime *const datetime)
{
    uint32_t time_reg = RTC->TR;
    uint32_t date_reg = RTC->DR;

    time_reg &= ~(RTC_TR_HT | RTC_TR_HU | RTC_TR_MNT | RTC_TR_MNU | RTC_TR_ST | RTC_TR_SU);
    date_reg &= ~(RTC_DR_YT | RTC_DR_YU | RTC_DR_WDU | RTC_DR_MT | RTC_DR_MU | RTC_DR_DT | RTC_DR_DU);

    RTC_SET_BCD(date_reg, RTC_DR_Y, datetime->year - 2000);
    RTC_SET_BCD(date_reg, RTC_DR_M, datetime->month);
    RTC_SET_BCD(date_reg, RTC_DR_D, datetime->day);
    RTC_SET_VAL(date_reg, RTC_DR_WDU, datetime->day_of_week);

    RTC_SET_BCD(time_reg, RTC_TR_H, datetime->hours);
    RTC_SET_BCD(time_reg, RTC_TR_MN, datetime->minutes);
    RTC_SET_BCD(time_reg, RTC_TR_S, datetime->seconds);

    if (enter_init_mode() < 0) {
        return -1;
    }

    RTC->TR = time_reg;
    RTC->DR = date_reg;

    exit_init_mode();

    return 0;
}

void
RTC_Init(void)
{
    disable_write_protection();
    if (enter_init_mode() == 0) {
        /* Set prescaler values for a 1 Hz clock */
        /* Required to do sync first, then async */
        RTC->PRER &= ~(RTC_PRER_ASYNC | RTC_PRER_SYNC);
        RTC->PRER |= 255u & RTC_PRER_SYNC;
        RTC->PRER |= (127u << RTC_PRER_ASYNC_SHIFT) & RTC_PRER_ASYNC;

        /* Set time format to 24-hour time */
        RTC->CR &= ~RTC_CR_FMT;
    }

    /* Setup a 1 Hz wakeup timer:
     *  - Output set to the wakeup timer
     *  - WUCKSEL set to RTCCLK / 2 (16.384 kHz)
     *  - WUTR set to 16384 (2 ^ 15)
     */
    disable_wut();
    RTC->CR |= (0x3 << RTC_CR_OSEL_SHIFT) & RTC_CR_OSEL;
    RTC->CR |= (0x3 << RTC_CR_WUCKSEL_SHIFT) & RTC_CR_WUCKSEL;
    RTC->WUTR &= ~0xffff;
    RTC->WUTR |= (1u << 15);
    enable_wut();

    exit_init_mode();
    enable_write_protection();
}

