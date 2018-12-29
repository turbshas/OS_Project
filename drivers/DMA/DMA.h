#ifndef _DMA_H
#define _DMA_H

#include "driver_common.h"

#define DMA1_BASE           (PERIPH_BASE + 0x26000)
#define DMA2_BASE           (PERIPH_BASE + 0x26400)
#ifdef __STM32F4xx__
#define DMA2D_BASE          (PERIPH_BASE + 0x2b000)
#endif

#define DMA_NUM_STREAMS 8
#define DMA_NUM_PRIO    4

struct dma_stream_regs {
    uint32_t CR;
    uint32_t NDTR;
    uint32_t PAR;
    uint32_t M0AR;
    uint32_t M1AR;
    uint32_t FCR;
};

struct DMA_Regs {
    uint32_t LISR;
    uint32_t HISR;
    uint32_t LIFCR;
    uint32_t HIFCR;
    struct dma_stream_regs stream[DMA_NUM_STREAMS];
};

typedef volatile struct DMA_Regs dma_t;

typedef struct _dma_request {
    /* These are interrupt handlers, might find a better way to support this
    void (*xfer_complete)(void);
    void (*half_xfer_complete)(void);
    void (*xfer_error)(void);
    void (*direct_mode_error)(void);
    void (*fifo_mode_error)(void);
    */
    uint8_t stream;
    uint8_t priority;
    uint8_t periph_xfer_size; /* byte, half-word, or word */
    uint8_t mem_xfer_size;
    uint8_t periph_burst;
    uint8_t mem_burst;
    uint8_t periph_inc;
    uint8_t mem_inc;
    uint8_t mode; /* CT, DBM, CIRC, and PFCTRL, FIFO or direct mode */
    uint8_t fifo_threshold;
} dma_request_t;

#define DMA_PRIO_LOW     0
#define DMA_PRIO_MED     1u
#define DMA_PRIO_HIGH    2u
#define DMA_PRIO_VHIGH   3u

#define DMA_XFERSIZE_BYTE  0
#define DMA_XFERSIZE_HWORD 1u
#define DMA_XFERSIZE_WORD  2u

#define DMA_BURST_NONE   0
#define DMA_BURST_INCR4  1u
#define DMA_BURST_INCR8  2u
#define DMA_BURST_INCR16 3u

#define DMA_INC_ENABLE (1u << 7)
#define DMA_PERIPH_INC_OFFSET_FIXED 1u

#define DMA_MODE_FIFO             (1u << 4)
#define DMA_MODE_CURR_TARGET      (1u << 3)
#define DMA_MODE_DOUBLE_BUFF      (1u << 2)
#define DMA_MODE_CIRC_MODE        (1u << 1)
#define DMA_MODE_PERIPH_FLOW_CTRL (1u << 0)

#define DMA_FIFO_THRESH_1QUARTER 0
#define DMA_FIFO_THRESH_HALF     1
#define DMA_FIFO_THRESH_3QUARTER 2
#define DMA_FIFO_THRESH_FULL     3

extern dma_t *const DMA1;
extern dma_t *const DMA2;
#ifdef __STM32F4xx__
extern dma_t *const DMA2D;
#endif

void DMA_Init(void);

#endif /* _DMA_H */

