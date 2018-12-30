#include "stm32_spi.h"

#define SPI1_BASE           (PERIPH_BASE + 0x13000)
#define SPI2_BASE           (PERIPH_BASE + 0x3800)
#define SPI3_BASE           (PERIPH_BASE + 0x3c00)

struct SPI_Regs {
    uint32_t CR1;
    uint32_t CR2;
    uint32_t SR;
    uint32_t DR;
    uint32_t CRCPR;
    uint32_t RXCRCR;
    uint32_t TXCRCR;
    uint32_t I2SCFGR;
    uint32_t I2SPR;
};

volatile struct SPI_Regs *const SPI1 = (void *)SPI1_BASE;
volatile struct SPI_Regs *const SPI2 = (void *)SPI2_BASE;
volatile struct SPI_Regs *const SPI3 = (void *)SPI3_BASE;

void
SPI_Init(void)
{
}

