#ifndef _SDIO_H
#define _SDIO_H

#include "driver_common.h"

#define SDIO_BASE           (PERIPH_BASE + 0x12c00)

struct SDIO_Regs {
    uint32_t POWER;
    uint32_t CLKCR;
    uint32_t ARG;
    uint32_t CMD;
    uint32_t RESPCMD;
    uint32_t RESP1;
    uint32_t RESP2;
    uint32_t RESP3;
    uint32_t RESP4;
    uint32_t DTIMER;
    uint32_t DLEN;
    uint32_t DCTRL;
    uint32_t DCOUNT;
    uint32_t STA;
    uint32_t ICR;
    uint32_t MASK;
    uint32_t FIFOCNT;
    uint32_t FIFO;
};

extern volatile struct SDIO_Regs *const SDIO;

void SDIO_Init(void);

#endif /* _SDIO_H */

