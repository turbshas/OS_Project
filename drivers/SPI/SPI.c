#include "SPI.h"

struct SPI_Regs *const SPI1 = (void *)SPI1_BASE;
struct SPI_Regs *const SPI2 = (void *)SPI2_BASE;
struct SPI_Regs *const SPI3 = (void *)SPI3_BASE;

/* Override IRQ Handlers to do nothing for now */
void SPI1_IRQHandler(void) {}
void SPI2_IRQHandler(void) {}
void SPI3_IRQHandler(void) {}

void
SPI_Init(void)
{
}

