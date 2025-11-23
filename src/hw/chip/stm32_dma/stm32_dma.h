#ifndef _DMA_H
#define _DMA_H

#include "chip_common.h"

#define DMA_NUM_STREAMS 8

/* Size of FIFO: 4 words/16 bytes */
/*
 * Data struct for parameters of the DMA request.
 *
 * mem1: Memory address 1 for DMA operation, memory address used for mem<->periph transfers.
 * mem2: Memory address 2, only used for mem<->mem transfers.
 * periph: Address of peripheral for DMA operation, used for mem<->periph transfers.
 * len: Length of data to transfer, in bytes.
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
struct DmaRequest {
    enum priority_level { PRIO_LOW = 0, PRIO_MED, PRIO_HIGH, PRIO_VHIGH, NUM_PRIOS };
    enum transfer_size { XFER_SIZE_BYTE = 0, XFER_SIZE_HWORD, XFER_SIZE_WORD, NUM_XFER_SIZES };
    enum burst_type { BURST_NONE = 0, BURST_INCR4, BURST_INCR8, BURST_INCR16, NUM_BURST_TYPES };
    enum periph_incr_mode { PERIPH_INCR_PSIZE = 0, PERIPH_INCR_FIXED, NUM_INCR_MODES };
    enum dma_mode { MODE_DIRECT = 0, MODE_PERIPH_FLOW_CTRL = 1, MODE_CIRC = 2, MODE_DOUBLE_BUFF = 4, MODE_CURR_TARGET = 8, MODE_FIFO = 16 };
    enum fifo_threshold_amt { FIFO_THRESH_1QUARTER = 0, FIFO_THRESH_HALF, FIFO_THRESH_3QUARTER, FIFO_THRESH_FULL, NUM_FIFO_THRESH };
    /* These are interrupt handlers, might find a better way to support this
    void (*xfer_complete)(void);
    void (*half_xfer_complete)(void);
    void (*xfer_error)(void);
    void (*direct_mode_error)(void);
    void (*fifo_mode_error)(void);
    */
    const void *mem1;
    const void *mem2;
    const volatile void *periph;
    uint32_t len;
    uint8_t stream;
    enum priority_level priority;
    enum transfer_size periph_xfer_size;
    enum transfer_size mem_xfer_size;
    enum burst_type periph_burst;
    enum burst_type mem_burst;
    bool periph_inc;
    enum periph_incr_mode periph_inc_offset;
    bool mem_inc;
    enum dma_mode mode; /* CT, DBM, CIRC, and PFCTRL, FIFO or direct mode */
    enum fifo_threshold_amt fifo_threshold;

    public:
        DmaRequest();
        void check_dma_req() const;
};

class DmaPeriph {
    uint32_t LISR;
    uint32_t HISR;
    uint32_t LIFCR;
    uint32_t HIFCR;
    struct dma_stream_regs {
        uint32_t CR;
        uint32_t NDTR;
        uint32_t PAR;
        uint32_t M0AR;
        uint32_t M1AR;
        uint32_t FCR;
    } streams[DMA_NUM_STREAMS];

    private:
        // These exist at specific locations in memory - should never be created
        DmaPeriph() = delete;
        ~DmaPeriph() = delete;
        DmaPeriph(const DmaPeriph&) = delete;
        DmaPeriph(DmaPeriph&&) = delete;

        void read_dma_request(struct dma_stream_regs &dest, const DmaRequest &req) volatile;
        void set_config(const uint8_t stream, const struct dma_stream_regs &stream_cfg) volatile;

    public:
        int periph_to_mem(const DmaRequest &req) volatile;
        int mem_to_periph(const DmaRequest &req) volatile;
        int mem_to_mem(const DmaRequest &req) volatile;
};

extern volatile DmaPeriph *const DMA1;
extern volatile DmaPeriph *const DMA2;
#ifdef __STM32F4xx__
extern volatile DmaPeriph *const DMA2D;
#endif

void DMA_Init(void);

#endif /* _DMA_H */

