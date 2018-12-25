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
 */
#include "DMA.h"

volatile struct DMA_Regs *const DMA1 = (void *)DMA1_BASE;
volatile struct DMA_Regs *const DMA2 = (void *)DMA2_BASE;
#ifdef __STM32F4xx__
volatile struct DMA_Regs *const DMA2D = (void *)DMA2D_BASE;
#endif

void
DMA_Init(void)
{
}

