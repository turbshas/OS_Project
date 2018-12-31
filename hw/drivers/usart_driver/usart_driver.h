#include "stm32_usart.h"

int usart_send_byte(usart_t usart, const char byte);
int usart_send_string(usart_t usart, const char *str, const uint8_t len);
void usart_driver_init(void);

