#include "stm32_dma.h"

#include "debug_stuff.h"

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

#define DMA_DIR_P2M 0
#define DMA_DIR_M2P 1u
#define DMA_DIR_M2M 2u

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
#define DMA1_BASE           (PERIPH_BASE + 0x26000)
#define DMA2_BASE           (PERIPH_BASE + 0x26400)
#ifdef __STM32F4xx__
#define DMA2D_BASE          (PERIPH_BASE + 0x2b000)
#endif

volatile DmaPeriph *const DMA1 = reinterpret_cast<volatile DmaPeriph *>(DMA1_BASE);
volatile DmaPeriph *const DMA2 = reinterpret_cast<volatile DmaPeriph *>(DMA2_BASE);
#ifdef __STM32F4xx__
volatile DmaPeriph *const DMA2D = reinterpret_cast<volatile DmaPeriph *>(DMA2D_BASE);
#endif

DmaRequest::DmaRequest()
    : mem1(nullptr),
    mem2(nullptr),
    periph(nullptr),
    len(0),

    stream(7),
    priority(PRIO_HIGH),
    periph_xfer_size(XFER_SIZE_BYTE),
    mem_xfer_size(XFER_SIZE_BYTE),
    periph_burst(BURST_NONE),
    mem_burst(BURST_NONE),
    periph_inc(false),
    periph_inc_offset(PERIPH_INCR_PSIZE),
    mem_inc(true),
    mode(MODE_DIRECT),
    fifo_threshold(FIFO_THRESH_1QUARTER)
{
}

void
DmaRequest::check_dma_req() const
{
    /* Make sure memory used is valid */
    const uintptr_t mem1_int = reinterpret_cast<uintptr_t>(mem1);
    assert(
            ((mem1_int >= SRAM_BASE) && (mem1_int < (SRAM_BASE + SRAM_SIZE)))
#ifdef __STM32F4xx__
            || ((mem1_int >= CCMRAM_BASE) && mem1_int < (CCMRAM_BASE + CCMRAM_SIZE))
#endif
            || ((mem1_int >= FLASH_BASE) && (mem1_int < (FLASH_BASE + FLASH_SIZE)))
          );
    if (mem2 != nullptr) {
        const uintptr_t mem2_int = reinterpret_cast<uintptr_t>(mem2);
        assert(
                ((mem2_int >= SRAM_BASE) && (mem2_int < (SRAM_BASE + SRAM_SIZE)))
#ifdef __STM32F4xx__
                || ((mem2_int >= CCMRAM_BASE) && mem2_int < (CCMRAM_BASE + CCMRAM_SIZE))
#endif
                || ((mem2_int >= FLASH_BASE) && (mem2_int < (FLASH_BASE + FLASH_SIZE)))
              );
    }
    if (periph != nullptr) {
        const uintptr_t periph_int = reinterpret_cast<uintptr_t>(periph);
        assert(periph_int >= PERIPH_BASE);
        assert(periph_int < (PERIPH_BASE + PERIPH_SIZE));
    }

    /* Check that values are within range */
    assert(stream < DMA_NUM_STREAMS);
    assert(priority < 4);
    assert(periph_xfer_size < 3);
    assert(mem_xfer_size < 3);
    assert(periph_burst < 4);
    assert(mem_burst < 4);
    assert(fifo_threshold < 4);

    /* Make sure memory is aligned */
    switch (mem_xfer_size) {
    case XFER_SIZE_WORD:
        assert((mem1_int & 0x3) == 0);
        break;
    case XFER_SIZE_HWORD:
        assert((mem1_int & 0x1) == 0);
        break;
    case XFER_SIZE_BYTE:
        break;
    default:
        /* I don't know how we got here */
        assert(0);
    }
    if (mem2 != nullptr) {
        const uintptr_t mem2_int = reinterpret_cast<uintptr_t>(mem2);
        switch (mem_xfer_size) {
        case XFER_SIZE_WORD:
            assert((mem2_int & 0x3) == 0);
            break;
        case XFER_SIZE_HWORD:
            assert((mem2_int & 0x1) == 0);
            break;
        case XFER_SIZE_BYTE:
            break;
        default:
            /* I don't know how we got here */
            assert(0);
        }
    }

    if (mode & MODE_PERIPH_FLOW_CTRL) {
        /*
         * Circular mode and double buffer mode aren't
         * allowed when the peripheral is the flow controller
         */
        assert((mode & MODE_CIRC) == 0);
        assert((mode & MODE_DOUBLE_BUFF) == 0);
    }

    if ((periph_burst > 0) || (mem_burst > 0)) {
        /* Burst transfers are only allowed in FIFO mode */
        assert((mode & MODE_FIFO) == MODE_FIFO);
    }

    /* Check FIFO/burst configuration */
    uint8_t fifo_size = 0;
    switch (fifo_threshold) {
    case FIFO_THRESH_1QUARTER: fifo_size = 4; break;
    case FIFO_THRESH_HALF:     fifo_size = 8; break;
    case FIFO_THRESH_3QUARTER: fifo_size = 12; break;
    case FIFO_THRESH_FULL:     fifo_size = 16; break;
    default: break;
    }

    uint8_t mem_read_size = 0;
    switch (mem_xfer_size) {
    case XFER_SIZE_BYTE:  mem_read_size = 1; break;
    case XFER_SIZE_HWORD: mem_read_size = 2; break;
    case XFER_SIZE_WORD:  mem_read_size = 4; break;
    default: break;
    }

    uint8_t mem_burst_amt = 0;
    switch (mem_burst) {
    case BURST_NONE:   mem_burst_amt = 1; break;
    case BURST_INCR4:  mem_burst_amt = 4; break;
    case BURST_INCR8:  mem_burst_amt = 8; break;
    case BURST_INCR16: mem_burst_amt = 16; break;
    default: break;
    }

    assert((mem_burst_amt * mem_read_size) <= fifo_size);

    /* Make sure the burst doesn't cross a 1 KB boundary */
    /* Maybe? idk if this is required */

    /* Check len parameter */
    /* NDTR stores number of *peripheral* transactions */
    uint8_t periph_read_size = 0;
    switch (periph_xfer_size) {
    case XFER_SIZE_BYTE:  periph_read_size = 1; break;
    case XFER_SIZE_HWORD: periph_read_size = 2; break;
    case XFER_SIZE_WORD:  periph_read_size = 4; break;
    default: break;
    }

    if ((periph_read_size != 1) || (mem_read_size != 1)) {
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
    if (mode & MODE_CURR_TARGET) {
        assert((num_transactions % (mem_burst_amt * (mem_read_size / periph_read_size))) == 0);
    }
}

/*
 * Reads a dma request and formats it to fit into the stream registers
 */
void
DmaPeriph::read_dma_request(struct dma_stream_regs &dest, const DmaRequest &req) volatile
{
    dest.CR = (streams[req.stream].CR & (~DMA_SxCR_ALL));
    dest.CR |= req.priority << DMA_SxCR_PL_SHIFT;
    dest.CR |= req.periph_xfer_size << DMA_SxCR_PSIZE_SHIFT;
    dest.CR |= req.mem_xfer_size << DMA_SxCR_MSIZE_SHIFT;
    dest.CR |= req.periph_burst << DMA_SxCR_PBURST_SHIFT;
    dest.CR |= req.mem_burst << DMA_SxCR_MBURST_SHIFT;
    if (req.periph_inc) {
        dest.CR |= DMA_SxCR_PINC;
        if (req.periph_inc_offset) {
            dest.CR |= DMA_SxCR_PINCOS;
        }
    }
    if (req.mem_inc) {
        dest.CR |= DMA_SxCR_MINC;
    }
    if (req.mode & DmaRequest::MODE_CURR_TARGET) {
        dest.CR |= DMA_SxCR_CT;
    }
    if (req.mode & DmaRequest::MODE_DOUBLE_BUFF) {
        dest.CR |= DMA_SxCR_DBM;
    }
    if (req.mode & DmaRequest::MODE_CIRC) {
        dest.CR |= DMA_SxCR_CIRC;
    }
    if (req.mode & DmaRequest::MODE_PERIPH_FLOW_CTRL) {
        dest.CR |= DMA_SxCR_PFCTRL;
    }

    dest.FCR = (streams[req.stream].FCR & (~DMA_SxFCR_ALL));
    if (req.mode & DmaRequest::MODE_FIFO) {
        dest.FCR |= DMA_SxFCR_DMDIS;
    }
    dest.FCR |= req.fifo_threshold << DMA_SxFCR_FTH_SHIFT;
}

void
DmaPeriph::set_config(const uint8_t stream, const struct dma_stream_regs &stream_cfg) volatile
{
    /* Disable DMA stream so we can modify the registers */
    streams[stream].CR &= ~DMA_SxCR_EN;
    /*
     * Sets the registers of the given stream to the given values.
     * Assumes the stream is already disabled.
     */
    streams[stream].CR = stream_cfg.CR;
    streams[stream].NDTR = stream_cfg.NDTR;
    streams[stream].PAR = stream_cfg.PAR;
    streams[stream].M0AR = stream_cfg.M0AR;
    streams[stream].M1AR = stream_cfg.M1AR;
    streams[stream].FCR = stream_cfg.FCR;
}

int
DmaPeriph::periph_to_mem(const DmaRequest &req) volatile
{
    const void *const mem = req.mem1;
    const volatile void *const periph = req.periph;
    const size_t len = req.len;

    /* Do debug error checking */
    req.check_dma_req();

    struct dma_stream_regs stream_cfg;

    read_dma_request(stream_cfg, req);

    uint8_t psize = 0;
    switch (req.periph_xfer_size) {
    case DmaRequest::XFER_SIZE_BYTE: psize = 1; break;
    case DmaRequest::XFER_SIZE_HWORD: psize = 2; break;
    case DmaRequest::XFER_SIZE_WORD: psize = 4; break;
    default: break;
    }
    stream_cfg.NDTR = len / psize;
    stream_cfg.PAR = (uintptr_t)periph;
    stream_cfg.M0AR = (uintptr_t)mem;
    stream_cfg.CR |= DMA_DIR_P2M << DMA_SxCR_DIR_SHIFT;

    set_config(req.stream, stream_cfg);

    return 0;
}

int
DmaPeriph::mem_to_periph(const DmaRequest &req) volatile
{
    const void *const mem = req.mem1;
    const volatile void *const periph = req.periph;
    const size_t len = req.len;

    /* Do debug error checking */
    req.check_dma_req();

    struct dma_stream_regs stream_cfg;

    read_dma_request(stream_cfg, req);

    uint8_t psize = 0;
    switch (req.periph_xfer_size) {
    case DmaRequest::XFER_SIZE_BYTE: psize = 1; break;
    case DmaRequest::XFER_SIZE_HWORD: psize = 2; break;
    case DmaRequest::XFER_SIZE_WORD: psize = 4; break;
    default: break;
    }
    stream_cfg.NDTR = len / psize;
    stream_cfg.PAR = (uintptr_t)periph;
    stream_cfg.M0AR = (uintptr_t)mem;
    stream_cfg.CR |= DMA_DIR_M2P << DMA_SxCR_DIR_SHIFT;

    set_config(req.stream, stream_cfg);
    streams[req.stream].CR |= DMA_SxCR_EN;

    return 0;
}

int
DmaPeriph::mem_to_mem(const DmaRequest &req) volatile
{
    const void *const mem1 = req.mem1;
    const void *const mem2 = req.mem2;
    const size_t len = req.len;

    /* Do debug error checking */
    req.check_dma_req();

    struct dma_stream_regs stream_cfg;

    read_dma_request(stream_cfg, req);

    uint8_t psize = 0;
    switch (req.periph_xfer_size) {
    case DmaRequest::XFER_SIZE_BYTE: psize = 1; break;
    case DmaRequest::XFER_SIZE_HWORD: psize = 2; break;
    case DmaRequest::XFER_SIZE_WORD: psize = 4; break;
    default: break;
    }
    stream_cfg.NDTR = len / psize;
    stream_cfg.PAR = (uintptr_t)mem1;
    stream_cfg.M0AR = (uintptr_t)mem2;
    stream_cfg.CR |= DMA_DIR_M2M << DMA_SxCR_DIR_SHIFT;

    set_config(req.stream, stream_cfg);

    return 0;
}

void
DMA_Init(void)
{
    /* Maybe dedicate some streams to some components/channels and init them here? */
}

