#ifndef _RTC_H
#define _RTC_H

#include "chip_common.h"

struct RTC_datetime {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t day_of_week;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
};

#define NUM_BKP_REG 20

class RtcPeriph {
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

    private:
        RtcPeriph() = delete;
        ~RtcPeriph() = delete;
        RtcPeriph(const RtcPeriph&) = delete;
        RtcPeriph(RtcPeriph&&) = delete;

        void disable_write_protection(void) volatile;
        void enable_write_protection(void) volatile;
        int enter_init_mode(void) volatile;
        void exit_init_mode(void) volatile;
        void disable_wut(void) volatile;
        void enable_wut(void) volatile;
        int wait_for_synchro(void) volatile;

    public:
        int get_datetime(struct RTC_datetime *const datetime) volatile;
        int set_datetime(const struct RTC_datetime *const datetime) volatile;
        int exit_dst(void) volatile;
        int enter_dst(void) volatile;
        void enable_WUT_Interrupt(void) volatile;
        void disable_WUT_Interrupt(void) volatile;
        void init(void) volatile;
};

extern volatile RtcPeriph *const RTC;

#endif /* _RTC_H */
