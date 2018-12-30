#include "stm32_i2c.h"

#define I2C1_BASE           (PERIPH_BASE + 0x5400)
#define I2C2_BASE           (PERIPH_BASE + 0x5800)
#define I2C3_BASE           (PERIPH_BASE + 0x5c00)

struct I2C_Regs {
    uint32_t CR1;
    uint32_t CR2;
    uint32_t OAR1;
    uint32_t OAR2;
    uint32_t DR;
    uint32_t SR1;
    uint32_t SR2;
    uint32_t CCR;
    uint32_t TRISE;
};

volatile struct I2C_Regs *const I2C1 = (void *)I2C1_BASE;
volatile struct I2C_Regs *const I2C2 = (void *)I2C2_BASE;
volatile struct I2C_Regs *const I2C3 = (void *)I2C3_BASE;

void
I2C_Init(void)
{
}

