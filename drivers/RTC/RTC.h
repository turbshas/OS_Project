#ifndef _RTC_H
#define _RTC_H

#include "driver_common.h"
#include "PWR.h"

#define RTC_BKP_BASE (PERIPH_BASE + 0x2800)

#define RTC_CR_FMT (1u << 6)

#define RTC_ISR_INIT (1u << 7)
#define RTC_ISR_INITF (1u << 6)

#define RTC_PRER_ASYNC 0x7f0000
#define RTC_PRER_ASYNC_SHIFT 16u
#define RTC_PRER_SYNC 0x1fff

#define NUM_BKP_REG 20

#define RTC_INIT_TIMEOUT 0x10000

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

extern volatile struct RTC_Regs *const RTC;

void RTC_Init(void);

#endif /* _RTC_H */
