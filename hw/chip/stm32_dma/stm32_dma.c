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
#include "stm32_dma.h"

#include "debug_stuff.h"

#define DMA1_BASE           (PERIPH_BASE + 0x26000)
#define DMA2_BASE           (PERIPH_BASE + 0x26400)
#ifdef __STM32F4xx__
#define DMA2D_BASE          (PERIPH_BASE + 0x2b000)
#endif

dma_t *const DMA1 = (void *)DMA1_BASE;
dma_t *const DMA2 = (void *)DMA2_BASE;
#ifdef __STM32F4xx__
dma_t *const DMA2D = (void *)DMA2D_BASE;
#endif

static inline void
check_dma_req(const void *const mem, const volatile void *const periph, const size_t len, const dma_request_t *const req)
{
    /* Check that values are within range */
    assert((uintptr_t)periph >= PERIPH_BASE);
    assert((uintptr_t)periph < (PERIPH_BASE + PERIPH_SIZE));
    assert(req->stream < DMA_NUM_STREAMS);
    assert(req->priority < 4);
    assert(req->periph_xfer_size < 3);
    assert(req->mem_xfer_size < 3);
    assert(req->periph_burst < 4);
    assert(req->mem_burst < 4);
    assert(req->fifo_threshold < 4);

    if (req->mode & DMA_MODE_PERIPH_FLOW_CTRL) {
        /*
         * Circular mode and double buffer mode aren't
         * allowed when the peripheral is the flow controller
         */
        assert((req->mode & DMA_MODE_CIRC_MODE) == 0);
        assert((req->mode & DMA_MODE_DOUBLE_BUFF) == 0);
    }

    if ((req->periph_burst > 0) || (req->mem_burst > 0)) {
        /* Burst transfers are only allowed in FIFO mode */
        assert((req->mode & DMA_MODE_FIFO) == DMA_MODE_FIFO);
    }

    /* Check FIFO/burst configuration */
    uint8_t fifo_size = 0;
    switch (req->fifo_threshold) {
    case DMA_FIFO_THRESH_1QUARTER: fifo_size = 4; break;
    case DMA_FIFO_THRESH_HALF:     fifo_size = 8; break;
    case DMA_FIFO_THRESH_3QUARTER: fifo_size = 12; break;
    case DMA_FIFO_THRESH_FULL:     fifo_size = 16; break;
    default: break;
    }

    uint8_t mem_read_size = 0;
    switch (req->mem_xfer_size) {
    case DMA_XFERSIZE_BYTE:  mem_read_size = 1; break;
    case DMA_XFERSIZE_HWORD: mem_read_size = 2; break;
    case DMA_XFERSIZE_WORD:  mem_read_size = 4; break;
    default: break;
    }

    uint8_t mem_burst = 0;
    switch (req->mem_burst) {
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
    switch (req->periph_xfer_size) {
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
    if (req->mode & DMA_MODE_CURR_TARGET) {
        assert((num_transactions % (mem_burst * (mem_read_size / periph_read_size))) == 0);
    }
}

/*
 * Reads a dma request and formats it to fit into the stream registers
 */
static inline void
read_dma_request(dma_t *const dma, const dma_request_t *const req, struct dma_stream_regs *const stream_cfg)
{
    stream_cfg->CR = (dma->stream[req->stream].CR & (~DMA_SxCR_ALL));
    stream_cfg->CR |= req->priority << DMA_SxCR_PL_SHIFT;
    stream_cfg->CR |= req->periph_xfer_size << DMA_SxCR_PSIZE_SHIFT;
    stream_cfg->CR |= req->mem_xfer_size << DMA_SxCR_MSIZE_SHIFT;
    stream_cfg->CR |= req->periph_burst << DMA_SxCR_PBURST_SHIFT;
    stream_cfg->CR |= req->mem_burst << DMA_SxCR_MBURST_SHIFT;
    if (req->periph_inc) {
        stream_cfg->CR |= DMA_SxCR_PINC;
        if (req->periph_inc_offset) {
            stream_cfg->CR |= DMA_SxCR_PINCOS;
        }
    }
    if (req->mem_inc) {
        stream_cfg->CR |= DMA_SxCR_MINC;
    }
    if (req->mode & DMA_MODE_CURR_TARGET) {
        stream_cfg->CR |= DMA_SxCR_CT;
    }
    if (req->mode & DMA_MODE_DOUBLE_BUFF) {
        stream_cfg->CR |= DMA_SxCR_DBM;
    }
    if (req->mode & DMA_MODE_CIRC_MODE) {
        stream_cfg->CR |= DMA_SxCR_CIRC;
    }
    if (req->mode & DMA_MODE_PERIPH_FLOW_CTRL) {
        stream_cfg->CR |= DMA_SxCR_PFCTRL;
    }

    stream_cfg->FCR = (dma->stream[req->stream].FCR & (~DMA_SxFCR_ALL));
    if (req->mode & DMA_MODE_FIFO) {
        stream_cfg->FCR |= DMA_SxFCR_DMDIS;
    }
    stream_cfg->FCR |= req->fifo_threshold << DMA_SxFCR_FTH_SHIFT;
}

static inline void
dma_set_config(dma_t *const dma, const uint8_t stream, const struct dma_stream_regs *const stream_cfg)
{
    /* Disable DMA stream so we can modify the registers */
    dma->stream[stream].CR &= ~DMA_SxCR_EN;
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

void
dma_req_init(dma_request_t *const req)
{
    req->stream = 7;
    req->priority = DMA_PRIO_HIGH;
    req->periph_xfer_size = DMA_XFERSIZE_BYTE;
    req->mem_xfer_size = DMA_XFERSIZE_BYTE;
    req->periph_burst = DMA_BURST_NONE;
    req->mem_burst = DMA_BURST_NONE;
    req->periph_inc = DMA_INC_DISABLE;
    req->periph_inc_offset = DMA_PERIPH_INC_OFFSET_PSIZE;
    req->mem_inc = DMA_INC_ENABLE;
    req->mode = 0; /* Direct mode */
    req->fifo_threshold = DMA_FIFO_THRESH_1QUARTER;
}

int
dma_periph_to_mem(dma_t *const dma, const void *const mem, const volatile void *const periph, const size_t len, const dma_request_t *const req)
{
    /* Do debug error checking */
    check_dma_req(mem, periph, len, req);

    struct dma_stream_regs stream_cfg;

    read_dma_request(dma, req, &stream_cfg);

    uint8_t psize = 0;
    switch (req->periph_xfer_size) {
    case DMA_XFERSIZE_BYTE: psize = 1; break;
    case DMA_XFERSIZE_HWORD: psize = 2; break;
    case DMA_XFERSIZE_WORD: psize = 4; break;
    default: break;
    }
    stream_cfg.NDTR = len / psize;
    stream_cfg.PAR = (uintptr_t)periph;
    stream_cfg.M0AR = (uintptr_t)mem;
    stream_cfg.CR |= DMA_DIR_P2M << DMA_SxCR_DIR_SHIFT;

    dma_set_config(dma, req->stream, &stream_cfg);

    return 0;
}

int
dma_mem_to_periph(dma_t *const dma, const void *const mem, const volatile void *const periph, const size_t len, const dma_request_t *const req)
{
    /* Do debug error checking */
    check_dma_req(mem, periph, len, req);

    struct dma_stream_regs stream_cfg;

    read_dma_request(dma, req, &stream_cfg);

    uint8_t psize = 0;
    switch (req->periph_xfer_size) {
    case DMA_XFERSIZE_BYTE: psize = 1; break;
    case DMA_XFERSIZE_HWORD: psize = 2; break;
    case DMA_XFERSIZE_WORD: psize = 4; break;
    default: break;
    }
    stream_cfg.NDTR = len / psize;
    stream_cfg.PAR = (uintptr_t)periph;
    stream_cfg.M0AR = (uintptr_t)mem;
    stream_cfg.CR |= DMA_DIR_M2P << DMA_SxCR_DIR_SHIFT;

    dma_set_config(dma, req->stream, &stream_cfg);
    dma->stream[req->stream].CR |= DMA_SxCR_EN;

    return 0;
}

int
dma_mem_to_mem(dma_t *const dma, const void *const mem1, const void *const mem2, const size_t len, const dma_request_t *const req)
{
    /* Do debug error checking */
    check_dma_req(mem1, mem2, len, req);

    struct dma_stream_regs stream_cfg;

    read_dma_request(dma, req, &stream_cfg);

    uint8_t psize = 0;
    switch (req->periph_xfer_size) {
    case DMA_XFERSIZE_BYTE: psize = 1; break;
    case DMA_XFERSIZE_HWORD: psize = 2; break;
    case DMA_XFERSIZE_WORD: psize = 4; break;
    default: break;
    }
    stream_cfg.NDTR = len / psize;
    stream_cfg.PAR = (uintptr_t)mem1;
    stream_cfg.M0AR = (uintptr_t)mem2;
    stream_cfg.CR |= DMA_DIR_M2M << DMA_SxCR_DIR_SHIFT;

    dma_set_config(dma, req->stream, &stream_cfg);

    return 0;
}

void
DMA_Init(void)
{
    /* Maybe dedicate some streams to some components/channels and init them here? */
}

