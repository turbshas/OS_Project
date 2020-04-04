#include "stm32_rtc.h"
#include "stm32_pwr.h"

#define RTC_BKP_BASE (PERIPH_BASE + 0x2800)

#define RTC_TR_HT                   0x300000
#define RTC_TR_HU                   0xf0000
#define RTC_TR_MNT                  0x7000
#define RTC_TR_MNU                  0xf00
#define RTC_TR_ST                   0x70
#define RTC_TR_SU                   0xf
#define RTC_TR_HT_SHIFT             20u
#define RTC_TR_HU_SHIFT             16u
#define RTC_TR_MNT_SHIFT            12u
#define RTC_TR_MNU_SHIFT            8u
#define RTC_TR_ST_SHIFT             4u
#define RTC_TR_SU_SHIFT             0

#define RTC_DR_YT                   0x300000
#define RTC_DR_YU                   0xf0000
#define RTC_DR_WDU                  0xe000
#define RTC_DR_MT                   0x1000
#define RTC_DR_MU                   0xf00
#define RTC_DR_DT                   0x30
#define RTC_DR_DU                   0xf
#define RTC_DR_YT_SHIFT             20u
#define RTC_DR_YU_SHIFT             16u
#define RTC_DR_WDU_SHIFT            13u
#define RTC_DR_MT_SHIFT             12u
#define RTC_DR_MU_SHIFT             8u
#define RTC_DR_DT_SHIFT             4u
#define RTC_DR_DU_SHIFT             0

#define RTC_CR_OSEL                 0x600000
#define RTC_CR_OSEL_SHIFT           21u
#define RTC_CR_BKP                  (1u << 18)
#define RTC_CR_SUB1H                (1u << 17)
#define RTC_CR_ADD1H                (1u << 16)
#define RTC_CR_WUTIE                (1u << 14)
#define RTC_CR_WUTE                 (1u << 10)
#define RTC_CR_FMT                  (1u << 6)
#define RTC_CR_WUCKSEL              0x7
#define RTC_CR_WUCKSEL_SHIFT        0

#define RTC_ISR_INIT                (1u << 7)
#define RTC_ISR_INITF               (1u << 6)
#define RTC_ISR_RSF                 (1u << 5)
#define RTC_ISR_WUTWF               (1u << 2)

#define RTC_PRER_ASYNC              0x7f0000
#define RTC_PRER_ASYNC_SHIFT        16u
#define RTC_PRER_SYNC               0x1fff

#define RTC_INIT_TIMEOUT            0x10000
#define RTC_SYNCHRO_TIMEOUT         0x80000

#define RTC_DOW_MONDAY              0x1
#define RTC_DOW_TUESDAY             0x2
#define RTC_DOW_WEDNESDAY           0x3
#define RTC_DOW_THURSDAY            0x4
#define RTC_DOW_FRIDAY              0x5
#define RTC_DOW_SATURDAY            0x6
#define RTC_DOW_SUNDAY              0x7

#define RTC_GET_VAL(reg, mask) (((reg) & (mask)) >> (mask##_SHIFT))
#define RTC_SET_VAL(reg, mask, val) ((reg) |= ((val) << (mask##_SHIFT)) & (mask))
#define RTC_GET_BCD(reg, mask) ((RTC_GET_VAL((reg), mask##T) * 10) + RTC_GET_VAL((reg), mask##U))
#define RTC_SET_BCD(reg, mask, val) \
    do { \
        RTC_SET_VAL((reg), mask##T, ((val) / 10)); \
        RTC_SET_VAL((reg), mask##U, ((val) % 10)); \
    } while (0)

volatile RtcPeriph *const RTC = reinterpret_cast<volatile RtcPeriph *>(RTC_BKP_BASE);

void
RtcPeriph::disable_write_protection(void) volatile
{
    PWR->disable_bd_write_protection();
    WPR |= 0xca;
    WPR |= 0x53;
}

void
RtcPeriph::enable_write_protection(void) volatile
{
    /* Any invalid key will re-enable write protection */
    WPR |= 0xff;
    PWR->enable_bd_write_protection();
}

int
RtcPeriph::enter_init_mode(void) volatile
{
    ISR |= RTC_ISR_INIT;

    /* Takes 1-2 RTCCLK cycles to actually enter initialization mode */
    unsigned counter = 0;
    while((ISR & RTC_ISR_INITF) == 0) {
        counter++;
        if (counter >= RTC_INIT_TIMEOUT) {
            return -1;
        }
    }
    return 0;
}

void
RtcPeriph::exit_init_mode(void) volatile
{
    ISR &= ~RTC_ISR_INIT;
}

void
RtcPeriph::disable_wut(void) volatile
{
    /* Disable WUT */
    CR &= ~RTC_CR_WUTE;

    /* Wait for write flag to go high */
    while ((ISR & RTC_ISR_WUTWF) == 0) { }
}

void
RtcPeriph::enable_wut(void) volatile
{
    CR |= RTC_CR_WUTE;
}

int
RtcPeriph::wait_for_synchro(void) volatile
{
    uint32_t counter = 0;

    while ((ISR & RTC_ISR_RSF) == 0) {
        if (counter == RTC_SYNCHRO_TIMEOUT) {
            return -1;
        }
        counter++;
    }

    return 0;
}

int
RtcPeriph::get_datetime(struct RTC_datetime *const datetime) volatile
{
    if ((ISR & RTC_ISR_RSF) == 0) {
        if (wait_for_synchro() < 0) {
            return -1;
        }
    }

    const uint32_t time_reg = TR;
    const uint32_t date_reg = DR;

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
RtcPeriph::set_datetime(const struct RTC_datetime *const datetime) volatile
{
    uint32_t time_reg = TR;
    uint32_t date_reg = DR;

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

    TR = time_reg;
    DR = date_reg;

    exit_init_mode();

    return 0;
}

int
RtcPeriph::exit_dst(void) volatile
{
    uint32_t time_reg = TR;

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
    CR |= RTC_CR_SUB1H;
    CR &= ~RTC_CR_BKP;

    return 0;
}

int
RtcPeriph::enter_dst(void) volatile
{
    uint32_t time_reg = TR;

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
    CR |= RTC_CR_ADD1H;
    CR |= RTC_CR_BKP;

    return 0;
}

void
RtcPeriph::enable_WUT_Interrupt(void) volatile
{
    CR |= RTC_CR_WUTIE;
}

void
RtcPeriph::disable_WUT_Interrupt(void) volatile
{
    CR &= ~RTC_CR_WUTIE;
}

void
RtcPeriph::init(void) volatile
{
    disable_write_protection();
    if (enter_init_mode() == 0) {
        /* Set prescaler values for a 1 Hz clock */
        /* Required to do sync first, then async */
        PRER &= ~(RTC_PRER_ASYNC | RTC_PRER_SYNC);
        PRER |= 255u & RTC_PRER_SYNC;
        PRER |= (127u << RTC_PRER_ASYNC_SHIFT) & RTC_PRER_ASYNC;

        /* Set time format to 24-hour time */
        CR &= ~RTC_CR_FMT;
    }

    /* Setup a 1 Hz wakeup timer:
     *  - Output set to the wakeup timer
     *  - WUCKSEL set to RTCCLK / 2 (16.384 kHz)
     *  - WUTR set to 16384 (2 ^ 15)
     */
    disable_wut();
    CR |= (0x3 << RTC_CR_OSEL_SHIFT) & RTC_CR_OSEL;
    CR |= (0x3 << RTC_CR_WUCKSEL_SHIFT) & RTC_CR_WUCKSEL;
    WUTR &= ~0xffff;
    WUTR |= (1u << 15);
    enable_wut();

    exit_init_mode();
    enable_write_protection();
}

