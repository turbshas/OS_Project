#ifndef _RTC_H
#define _RTC_H

#include "driver_common.h"
#include "PWR.h"

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

#define NUM_BKP_REG                 20

#define RTC_INIT_TIMEOUT            0x10000
#define RTC_SYNCHRO_TIMEOUT         0x80000

#define RTC_DOW_MONDAY              0x1
#define RTC_DOW_TUESDAY             0x2
#define RTC_DOW_WEDNESDAY           0x3
#define RTC_DOW_THURSDAY            0x4
#define RTC_DOW_FRIDAY              0x5
#define RTC_DOW_SATURDAY            0x6
#define RTC_DOW_SUNDAY              0x7

#define RTC_GET_VAL(reg, mask) ((reg & mask) >> mask##_SHIFT)
#define RTC_GET_BCD(reg, val) ((RTC_GET_VAL(reg, val##T) * 10) + RTC_GET_VAL(reg, val##U))

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

struct RTC_datetime {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t day_of_week;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
};

extern volatile struct RTC_Regs *const RTC;

int RTC_get_datetime(struct RTC_datetime *const datetime);
void RTC_Init(void);

#endif /* _RTC_H */
