#ifndef _RTC_H
#define _RTC_H

#include "chip_common.h"
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

#define RTC_GET_VAL(reg, mask) (((reg) & (mask)) >> (mask##_SHIFT))
#define RTC_SET_VAL(reg, mask, val) ((reg) |= ((val) << (mask##_SHIFT)) & (mask))
#define RTC_GET_BCD(reg, mask) ((RTC_GET_VAL((reg), mask##T) * 10) + RTC_GET_VAL((reg), mask##U))
#define RTC_SET_BCD(reg, mask, val) \
    do { \
        RTC_SET_VAL((reg), mask##T, ((val) / 10)); \
        RTC_SET_VAL((reg), mask##U, ((val) % 10)); \
    } while (0)

struct RTC_datetime {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t day_of_week;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
};

int RTC_get_datetime(struct RTC_datetime *const datetime);
int RTC_set_datetime(const struct RTC_datetime *const datetime);
int RTC_exit_dst(void);
int RTC_enter_dst(void);
void RTC_Enable_WUT_Interrupt(void);
void RTC_Disable_WUT_Interrupt(void);
void RTC_Init(void);

#endif /* _RTC_H */
