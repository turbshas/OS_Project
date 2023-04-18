#include "savedRegisters.hpp"

AutomaticallyStackedRegisters::AutomaticallyStackedRegisters()
    : R0(),
      R1(),
      R2(),
      R3(),
      R12(),
      LR(),
      PC(),
      PSR()
{
}

SavedRegisters::SavedRegisters()
    : R4(0x0),
      R5(0x0),
      R6(0x0),
      R7(0x0),
      R8(0x0),
      R9(0x0),
      R10(0x0),
      R11(0x0),
      SP(0x0),
      ExceptionLR(0x0)
{
}
