#include "SPI.h"

volatile struct SPI_Regs *const SPI1 = (void *)SPI1_BASE;
volatile struct SPI_Regs *const SPI2 = (void *)SPI2_BASE;
volatile struct SPI_Regs *const SPI3 = (void *)SPI3_BASE;

void
SPI_Init(void)
{
}

