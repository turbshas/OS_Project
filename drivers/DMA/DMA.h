#ifndef _DMA_H
#define _DMA_H

#include "driver_common.h"

#define DMA1_BASE           (PERIPH_BASE + 0x26000)
#define DMA2_BASE           (PERIPH_BASE + 0x26400)
#ifdef __STM32F4xx__
#define DMA2D_BASE          (PERIPH_BASE + 0x2b000)
#endif

struct DMA_Regs {
    uint32_t LISR;
    uint32_t HISR;
    uint32_t LIFCR;
    uint32_t HIFCR;
    struct {
        uint32_t CR;
        uint32_t NDTR;
        uint32_t PAR;
        uint32_t M0AR;
        uint32_t M1AR;
        uint32_t FCR;
    } stream[8];
};

extern volatile struct DMA_Regs *const DMA1 = (void *)DMA1_BASE;
extern volatile struct DMA_Regs *const DMA2 = (void *)DMA2_BASE;
#ifdef __STM32F4xx__
extern volatile struct DMA_Regs *const DMA2D = (void *)DMA2D_BASE;
#endif

void DMA_Init(void);

#endif /* _DMA_H */

