#include "cpuRegsOnStack.h"

CpuRegsOnStack::CpuRegsOnStack()
    : R4(0x0),
    R5(0x0),
    R6(0x0),
    R7(0x0),
    R8(0x0),
    R9(0x0),
    R10(0x0),
    R11(0x0),
    R0(0x0),
    R1(0x0),
    R2(0x0),
    R3(0x0),
    R12(0x0),
    LR(0x0),
    PC(0x0),
    PSR(0x0)
{
}

CpuRegsOnStack::CpuRegsOnStack(const CpuRegsOnStack &source)
    : R4(source.R4),
    R5(source.R5),
    R6(source.R6),
    R7(source.R7),
    R8(source.R8),
    R9(source.R9),
    R10(source.R10),
    R11(source.R11),
    R0(source.R0),
    R1(source.R1),
    R2(source.R2),
    R3(source.R3),
    R12(source.R12),
    LR(source.LR),
    PC(source.PC),
    PSR(source.PSR)
{
}

CpuRegsOnStack::CpuRegsOnStack(CpuRegsOnStack &&source)
    : CpuRegsOnStack(const_cast<const CpuRegsOnStack &>(source))
{
}
