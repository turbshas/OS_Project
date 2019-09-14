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

    struct dma_request dma_req = {
        .stream = 0,
        .priority = DMA_PRIO_LOW,
        .periph_xfer_size = DMA_XFERSIZE_BYTE,
        .mem_xfer_size = DMA_XFERSIZE_BYTE,
        .periph_burst = DMA_BURST_NONE,
        .mem_burst = DMA_BURST_NONE,
        .periph_inc = DMA_INC_DISABLE,
        .periph_inc_offset = DMA_PERIPH_INC_OFFSET_PSIZE,
        .mem_inc = DMA_INC_ENABLE,
        .mode = DMA_MODE_DIRECT,
        .fifo_threshold = DMA_FIFO_THRESH_1QUARTER,
    };
    dma_mem_to_periph(DMA1, str, &usart->DR, len, &dma_req);
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

