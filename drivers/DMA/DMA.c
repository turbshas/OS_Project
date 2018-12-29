/*
 * Things to consider:
 *  - Periph to Mem
 *  - Mem to Periph
 *  - Mem to Mem (DMA2 only)
 *  - Double buffer mode (only available on periph to mem/mem to periph)
 *  - Burst mode (has specific requirements w.r.t. psize, msize, and fifo fill threshold)
 *  - Current target of double buffer mode
 *  - Priority level of stream
 *  - Channel select of stream
 *  - Periph and mem increment amount (either based on psize/msize or set to a multiple of 4 bytes)
 *  - Enable/disable address increment
 *  - Circular mode
 *  - Flow controller (probably always DMA, but will have to investigate)
 *  - Interrupts for each stream (probably want a way for the caller to use their own interrup)
 *  - Amount of data to be transferred (DMA needs # of transactions, probably accept # of bytes and convert that # of transactions
 *  in the function
 *  - Periph/mem address
 *  - FIFO/direct mode
 *  - Will have to figure out how channels/streams are connected to each of the devices attached to the DMA
 *  - Transfer suspension
 *  - Which stream is wanted, which channel is wanted
 */
/*
 * Plans for implementation:
 *  - Config structure init functions
 *  - Big config struct to configure DMA parameters
 *  - Periph to mem, mem to periph, and mem to mem functions
 *  - Maybe reserve some streams by default for peripherals with preloaded config where a similar config is used often
 *  - Allow configurable interrupts (setup each of the DMA interrupt to call a user-configurable interrupt)
 */
#include "DMA.h"

#include "debug_stuff.h"

dma_t *const DMA1 = (void *)DMA1_BASE;
dma_t *const DMA2 = (void *)DMA2_BASE;
#ifdef __STM32F4xx__
dma_t *const DMA2D = (void *)DMA2D_BASE;
#endif

static inline void
check_dma_req(const void *const src, const void *const dest, const size_t len, const dma_request_t *const stream_cfg)
{
    /* Check that values are within range */
    assert(stream_cfg->stream < DMA_NUM_STREAMS);
    assert(stream_cfg->priority < 4);
    assert(stream_cfg->periph_xfer_size < 3);
    assert(stream_cfg->mem_xfer_size < 3);
    assert(stream_cfg->periph_burst < 4);
    assert(stream_cfg->mem_burst < 4);
    assert(stream_cfg->fifo_threshold < 4);

    if (stream_cfg->mode & DMA_MODE_PERIPH_FLOW_CTRL) {
        /*
         * Circular mode and double buffer mode aren't
         * allowed when the peripheral is the flow controller
         */
        assert((stream_cfg->mode & DMA_MODE_CIRC_MODE) == 0);
        assert((stream_cfg->mode & DMA_MODE_DOUBLE_BUFF) == 0);
    }

    if ((stream_cfg->periph_burst > 0) || (stream_cfg->mem_burst > 0)) {
        /* Burst transfers are only allowed in FIFO mode */
        assert((stream_cfg->mode & DMA_MODE_FIFO) == DMA_MODE_FIFO);
    }

    /* Check FIFO/burst configuration */
    uint8_t fifo_size = 0;
    switch (stream_cfg->fifo_threshold) {
    case DMA_FIFO_THRESH_1QUARTER: fifo_size = 4; break;
    case DMA_FIFO_THRESH_HALF:     fifo_size = 8; break;
    case DMA_FIFO_THRESH_3QUARTER: fifo_size = 12; break;
    case DMA_FIFO_THRESH_FULL:     fifo_size = 16; break;
    default: break;
    }

    uint8_t mem_read_size = 0;
    switch (stream_cfg->mem_xfer_size) {
    case DMA_XFERSIZE_BYTE:  mem_read_size = 1; break;
    case DMA_XFERSIZE_HWORD: mem_read_size = 2; break;
    case DMA_XFERSIZE_WORD:  mem_read_size = 4; break;
    default: break;
    }

    uint8_t mem_burst = 0;
    switch (stream_cfg->mem_burst) {
    case DMA_BURST_NONE:   mem_burst = 1; break;
    case DMA_BURST_INCR4:  mem_burst = 4; break;
    case DMA_BURST_INCR8:  mem_burst = 8; break;
    case DMA_BURST_INCR16: mem_burst = 16; break;
    default: break;
    }

    assert((mem_burst * mem_read_size) <= fifo_size);

    /* Make sure the burst doesn't cross a 1 KB boundary */
    /* Maybe? idk if this is required */

    /* Check len parameter */
    /* NDTR stores number of *peripheral* transactions */
    uint8_t periph_read_size = 0;
    switch (stream_cfg->periph_xfer_size) {
    case DMA_XFERSIZE_BYTE:  periph_read_size = 1; break;
    case DMA_XFERSIZE_HWORD: periph_read_size = 2; break;
    case DMA_XFERSIZE_WORD:  periph_read_size = 4; break;
    default: break;
    }

    if ((periph_read_size != DMA_XFERSIZE_BYTE) || (mem_read_size != DMA_XFERSIZE_BYTE)) {
        /*
         * Instead of checking buffer boundaries and other things,
         * only allow odd-length transfers with a byte-to-byte transfer size
         */
        assert((len & 0x1) == 0);
    }
    uint16_t num_transactions = len / periph_read_size;
    if (periph_read_size > mem_read_size) {
        /*
         * If (PSIZE, MSIZE) equals:
         *  - (8-bit, 16-bit) or (16-bit, 32-bit)
         *    then NDTR needs to be a multiple of 2
         *  - (8-bit, 32-bit) then NDTR needs to be
         *    a multiple of 4
         */
        assert((num_transactions % (periph_read_size / mem_read_size)) == 0);
    }

    /* Check NDTR for circular mode */
    if (stream_cfg->mode & DMA_MODE_CURR_TARGET) {
        assert((num_transactions % (mem_burst * (mem_read_size / periph_read_size))) == 0);
    }
}

static inline void
dma_set_config(dma_t *dma, uint8_t stream, struct dma_stream_regs *stream_cfg)
{
    /*
     * Sets the registers of the given stream to the given values.
     * Assumes the stream is already disabled.
     */
    dma->stream[stream].CR = stream_cfg->CR;
    dma->stream[stream].NDTR = stream_cfg->NDTR;
    dma->stream[stream].PAR = stream_cfg->PAR;
    dma->stream[stream].M0AR = stream_cfg->M0AR;
    dma->stream[stream].M1AR = stream_cfg->M1AR;
    dma->stream[stream].FCR = stream_cfg->FCR;
}

int
dma_periph_to_mem(dma_t *dma, void *mem, void *periph, size_t len, dma_request_t *req)
{
    /* Do debug error checking */
    check_dma_req(mem, periph, len, req);
    assert((uintptr_t)periph >= PERIPH_BASE);
    assert((uintptr_t)periph > (PERIPH_BASE + PERIPH_SIZE));

    struct dma_stream_regs stream_cfg;

    stream_cfg.PAR = (uintptr_t)periph;
    stream_cfg.M0AR = (uintptr_t)mem;

    return 0;
}

int
dma_mem_to_periph(dma_t *dma, void *mem, void *periph, size_t len, dma_request_t *req)
{
    return 0;
}

int
dma_mem_to_mem(dma_t *dma, void *mem1, void *mem2, size_t len, dma_request_t *req)
{
    return 0;
}

void
DMA_Init(void)
{
    /* Maybe dedicate some streams to some components/channels and init them here? */
}

