#include "SDIO.h"

struct SDIO_Regs *const SDIO = (void *)SDIO_BASE;

/* Override IRQ to do nothing for now */
void SDIO_IRQHandler(void) {}

void
SDIO_Init(void)
{
}

