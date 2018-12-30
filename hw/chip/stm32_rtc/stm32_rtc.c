#include "stm32_rtc.h"
#include "stm32_pwr.h"

#define RTC_BKP_BASE (PERIPH_BASE + 0x2800)

struct RTC_Regs {
    uint32_t TR;
    uint32_t DR;
    uint32_t CR;
    uint32_t ISR;
    uint32_t PRER;
    uint32_t WUTR;
    uint32_t CALIBR;
    uint32_t ALRMAR;
    uint32_t ALRMBR;
    uint32_t WPR;
    uint32_t TSTR;
    uint32_t TSDR;
    uint32_t TAFCR;
    uint32_t BKPR[NUM_BKP_REG];
};

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

int
RTC_exit_dst(void)
{
    uint32_t time_reg = RTC->TR;

    if (RTC_GET_BCD(time_reg, RTC_TR_H) == 0) {
        /* Don't want to subtract when the hour is 0 */
        return -1;
    }

    if ((RTC_GET_BCD(time_reg, RTC_TR_MN) == 59) && (RTC_GET_BCD(time_reg, RTC_TR_S) > 55)) {
        /*
         * It is recommended not to perform the dst change when the hour
         * is changing as to not mask the change of the hour.
         * Giving a 5 second window here.
         */
        return -1;
    }
    /*
     * Subtract an hour, then clear bkp bit.
     * Bkp bit = 0 -> not in DST
     */
    RTC->CR |= RTC_CR_SUB1H;
    RTC->CR &= ~RTC_CR_BKP;

    return 0;
}

int
RTC_enter_dst(void)
{
    uint32_t time_reg = RTC->TR;

    if (RTC_GET_BCD(time_reg, RTC_TR_H) == 23) {
        /* Don't want to add when the hour is 23 */
        return -1;
    }

    if ((RTC_GET_BCD(time_reg, RTC_TR_MN) == 59) && (RTC_GET_BCD(time_reg, RTC_TR_S) > 55)) {
        /*
         * It is recommended not to perform the dst change when the hour
         * is changing as to not mask the change of the hour.
         * Giving a 5 second window here.
         */
        return -1;
    }
    /*
     * Add an hour, then set bkp bit.
     * Bkp bit = 1 -> currently in DST
     */
    RTC->CR |= RTC_CR_ADD1H;
    RTC->CR |= RTC_CR_BKP;

    return 0;
}

void
RTC_Enable_WUT_Interrupt(void)
{
    RTC->CR |= RTC_CR_WUTIE;
}

void
RTC_Disable_WUT_Interrupt(void)
{
    RTC->CR &= ~RTC_CR_WUTIE;
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
