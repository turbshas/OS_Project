#include "usart_driver.h"

int
usart_send_byte(usart_t usart, const char byte)
{
    usart_enable(usart);

    usart_send(usart, byte);
    usart_finish_send(usart);

    usart_disable(usart);

    return 0;
}

int
usart_send_string(usart_t usart, const char *const str, const uint8_t len)
{
    int count = 0;

    usart_enable(usart);

    struct dma_request dma_req;
    dma_req.mem1 = static_cast<const void *>(str);
    dma_req.periph = &usart->DR;
    dma_req.len = len;

    dma_req.stream = 0,
    dma_req.priority = DMA_PRIO_LOW,
    dma_req.periph_xfer_size = DMA_XFERSIZE_BYTE,
    dma_req.mem_xfer_size = DMA_XFERSIZE_BYTE,
    dma_req.periph_burst = DMA_BURST_NONE,
    dma_req.mem_burst = DMA_BURST_NONE,
    dma_req.periph_inc = DMA_INC_DISABLE,
    dma_req.periph_inc_offset = DMA_PERIPH_INC_OFFSET_PSIZE,
    dma_req.mem_inc = DMA_INC_ENABLE,
    dma_req.mode = DMA_MODE_DIRECT,
    dma_req.fifo_threshold = DMA_FIFO_THRESH_1QUARTER,

    DMA1->mem_to_periph(dma_req);
#if 0
    for (int i = 0; i < len; i++) {
        if ((unsigned char)str[i] > 0xff) {
            return count;
        }
        usart_send(usart, str[i]);
        count++;
    }

    usart_finish_send(usart);
    usart_disable(usart);
#endif

    return count;
}

void
usart_driver_init(void)
{
    usart_init(USART3);
}

