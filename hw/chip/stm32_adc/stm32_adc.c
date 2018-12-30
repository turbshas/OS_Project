#include "stm32_adc.h"

#define ADC_BASE            (PERIPH_BASE + 0x12000)

struct _ADC_Reg {
    uint32_t SR;
    uint32_t CR1;
    uint32_t CR2;
    uint32_t SMPR1;
    uint32_t SMPR2;
    uint32_t JOFR1;
    uint32_t JOFR2;
    uint32_t JOFR3;
    uint32_t JOFR4;
    uint32_t HTR;
    uint32_t LTR;
    uint32_t SQR1;
    uint32_t SQR2;
    uint32_t SQR3;
    uint32_t JSQR;
    uint32_t JDR1;
    uint32_t JDR2;
    uint32_t JDR3;
    uint32_t JDR4;
    uint32_t DR;
};

struct ADC_Common_Regs {
    uint32_t CSR;
    uint32_t CCR;
    uint32_t CDR;
};

struct ADC_Regs {
    struct {
        volatile struct _ADC_Reg regs;
        uint32_t reserved[44];
    } ADCs[3];
    volatile struct ADC_Common_Regs Common;

#define ADC1 ADCs[0].regs
#define ADC2 ADCs[1].regs
#define ADC3 ADCs[2].regs
};

struct ADC_Regs *const ADC = (void *)ADC_BASE;

void
ADC_Init(void)
{
}

