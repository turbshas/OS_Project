#ifndef _DMA_H
#define _DMA_H

#include "chip_common.h"

#define DMA_SxCR_CHSEL_SHIFT    25u
#define DMA_SxCR_MBURST_SHIFT   23u
#define DMA_SxCR_PBURST_SHIFT   21u
#define DMA_SxCR_PL_SHIFT       16u
#define DMA_SxCR_MSIZE_SHIFT    13u
#define DMA_SxCR_PSIZE_SHIFT    11u
#define DMA_SxCR_DIR_SHIFT      6u

#define DMA_SxCR_CHSEL      (7u << DMA_SxCR_CHSEL_SHIFT)
#define DMA_SxCR_MBURST     (3u << DMA_SxCR_MBURST_SHIFT)
#define DMA_SxCR_PBURST     (3u << DMA_SxCR_PBURST_SHIFT)
#define DMA_SxCR_CT         (1u << 19)
#define DMA_SxCR_DBM        (1u << 18)
#define DMA_SxCR_PL         (3u << DMA_SxCR_PL_SHIFT)
#define DMA_SxCR_PINCOS     (1u << 15)
#define DMA_SxCR_MSIZE      (3u << DMA_SxCR_MSIZE_SHIFT)
#define DMA_SxCR_PSIZE      (3u << DMA_SxCR_PSIZE_SHIFT)
#define DMA_SxCR_MINC       (1u << 10)
#define DMA_SxCR_PINC       (1u << 9)
#define DMA_SxCR_CIRC       (1u << 8)
#define DMA_SxCR_DIR        (3u << DMA_SxCR_DIR_SHIFT)
#define DMA_SxCR_PFCTRL     (1u << 5)
#define DMA_SxCR_TCIE       (1u << 4)
#define DMA_SxCR_HTIE       (1u << 3)
#define DMA_SxCR_TEIE       (1u << 2)
#define DMA_SxCR_DMEIE      (1u << 1)
#define DMA_SxCR_EN         (1u << 0)
#define DMA_SxCR_ALL        0xfefffff

#define DMA_SxFCR_FS_SHIFT  3u
#define DMA_SxFCR_FTH_SHIFT 0

#define DMA_SxFCR_FEIE      (1u << 7)
#define DMA_SxFCR_FS        (7u << DMA_SxFCR_FS_SHIFT)
#define DMA_SxFCR_DMDIS     (1u << 2)
#define DMA_SxFCR_FTH       (3u << DMA_SxFCR_FTH_SHIFT)
#define DMA_SxFCR_ALL       0xbf

#define DMA_NUM_STREAMS 8
#define DMA_NUM_PRIO    4

#define DMA_DIR_P2M 0
#define DMA_DIR_M2P 1u
#define DMA_DIR_M2M 2u

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

/* Size of FIFO: 4 words/16 bytes */
/*
 * Data struct for parameters of the DMA request.
 *
 * stream: Which stream to use: 0 to 7
 * priority: Priority of the request: low, med, high, or very high
 * periph_xfer_size: Size of peripheral reads/writes: byte, half-word, or word
 * mem_xfer_size: Size of memory reads/writes: byte, half-word, word
 * periph_burst: Whether to perform burst transfers on the peripheral, in beats of 1 (disabled), 4, 8, or 16. Must not exceed size of FIFO
 * mem_burst: Whether to perform burst transfers on the memory, in beats of 1 (disabled), 4, 8, or 16. Must not exceed size of FIFO
 *  - Burst transfer can't cross 1 KB boundaries
 *  - Burst transfers must not fill FIFO past threshold
 * periph_inc: Whether to increment the peripheral pointer after each read/write and if so, whether to increment by a fixed amount (4 bytes) or by the xfer size
 * periph_inc_offset: Offset to the fixed increment before reading (fixed increment aligns to 4 bytes)
 * mem_inc: Whether to increment the memory pointer after each read/write
 * mode: Mode in which DMA will operate:
 *  - FIFO: fill FIFO up to specified threshold before writing to dest
 *  - Direct: Read directly from src to dest
 *  - Circular: number of transfers is automatically reloaded, service continues
 *    - NDTR = multiple of Mem burst size * (MSize/ PSize)
 *  - Double Buffer: src/dest are both memory, circular mode is enabled, mem pointers are swapped at each end of transaction
 *    - If CT is 1, can change pointers on the fly
 *  - Preipheral flow control: peripheral is the master in the DMA transfers
 *    - Only valid with SDIO peripheral
 *  - Direct, Circular, Double Buffer disallowed with mem-to-mem
 *  - Circular, Double Buffer disallowed with peripheral as flow controller
 * fifo_threshold: When in FIFO mode, at what point to transfer from the FIFO to the target: 1/4 full, 1/2 full, 3/4 full, or completely full
 */
struct dma_request {
    /* These are interrupt handlers, might find a better way to support this
    void (*xfer_complete)(void);
    void (*half_xfer_complete)(void);
    void (*xfer_error)(void);
    void (*direct_mode_error)(void);
    void (*fifo_mode_error)(void);
    */
    uint8_t stream;
    uint8_t priority;
    uint8_t periph_xfer_size;
    uint8_t mem_xfer_size;
    uint8_t periph_burst;
    uint8_t mem_burst;
    uint8_t periph_inc;
    uint8_t periph_inc_offset;
    uint8_t mem_inc;
    uint8_t mode; /* CT, DBM, CIRC, and PFCTRL, FIFO or direct mode */
    uint8_t fifo_threshold;
};

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

#define DMA_INC_DISABLE 0
#define DMA_INC_ENABLE 1u
#define DMA_PERIPH_INC_OFFSET_PSIZE 0
#define DMA_PERIPH_INC_OFFSET_FIXED 1u

#define DMA_MODE_FIFO             (1u << 4)
#define DMA_MODE_CURR_TARGET      (1u << 3)
#define DMA_MODE_DOUBLE_BUFF      (1u << 2)
#define DMA_MODE_CIRC_MODE        (1u << 1)
#define DMA_MODE_PERIPH_FLOW_CTRL (1u << 0)
#define DMA_MODE_DIRECT           0u

#define DMA_FIFO_THRESH_1QUARTER 0
#define DMA_FIFO_THRESH_HALF     1
#define DMA_FIFO_THRESH_3QUARTER 2
#define DMA_FIFO_THRESH_FULL     3

extern dma_t *const DMA1;
extern dma_t *const DMA2;
#ifdef __STM32F4xx__
extern dma_t *const DMA2D;
#endif

void dma_req_init(struct dma_request *const req);
int dma_periph_to_mem(dma_t *const dma, const void *const mem, const volatile void *const periph, const size_t len, const struct dma_request *const req);
int dma_mem_to_periph(dma_t *const dma, const void *const mem, const volatile void *const periph, const size_t len, const struct dma_request *const req);
int dma_mem_to_mem(dma_t *const dma, const void *const mem1, const void *const mem2, const size_t len, const struct dma_request *const req);
void DMA_Init(void);

#endif /* _DMA_H */

