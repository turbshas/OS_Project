#include "usart_driver.h"

int
usart_send_byte(usart_t usart, const char byte)
{
    usart->enable();

    usart->send(byte);
    usart->finish_send();

    usart->disable();

    return 0;
}

int
usart_send_string(usart_t usart, const char* const str, const uint8_t len)
{
    int count = 0;

    usart->enable();

    DmaRequest dma_req;
    dma_req.mem1 = static_cast<const void*>(str);
    dma_req.periph = usart->get_address_for_dma();
    dma_req.len = len;

    dma_req.stream = 0;
    dma_req.priority = DmaRequest::PRIO_LOW;

    // DMA1->mem_to_periph(dma_req);
#if 1
    for (int i = 0; i < len; i++)
    {
        if ((unsigned char)str[i] > 0xff)
        {
            return count;
        }
        usart->send(str[i]);
        count++;
    }

    usart->finish_send();
    usart->disable();
#endif

    return count;
}

void
usart_driver_init(void)
{
    USART1->init();
    USART2->init();
    USART3->init();
    // UART4->init();
    // UART5->init();
    USART6->init();
    // UART7->init();
    // UART8->init();
    // UART9->init();
    // UART10->init();
}
