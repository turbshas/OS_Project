#include "I2C.h"

struct I2C_Regs *const I2C1 = (void *)I2C1_BASE;
struct I2C_Regs *const I2C2 = (void *)I2C2_BASE;
struct I2C_Regs *const I2C3 = (void *)I2C3_BASE;

/* Override IRQ handlers to do nothing for now */
void I2C1_EV_IRQHandler(void) {}
void I2C2_EV_IRQHandler(void) {}
void I2C3_EV_IRQHandler(void) {}

void
I2C_Init(void)
{
}

