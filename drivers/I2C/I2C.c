#include "I2C.h"

volatile struct I2C_Regs *const I2C1 = (void *)I2C1_BASE;
volatile struct I2C_Regs *const I2C2 = (void *)I2C2_BASE;
volatile struct I2C_Regs *const I2C3 = (void *)I2C3_BASE;

void
I2C_Init(void)
{
}

