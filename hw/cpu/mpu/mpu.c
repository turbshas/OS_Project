#include "mpu.h"

#define MPU_BASE 0xe000ed90

#define MPU_RBAR_ADDR 0xfffffffe0
#define MPU_RBAR_VALID (1u << 4)
#define MPU_RBAR_REGION 0x7

#define MPU_RASR_ATTR 0xffff0000
#define MPU_RASR_SRD_SIZE 0xffff
#define MPU_RASR_XN (1u << 28)
#define MPU_RASR_AP 0x7000000
#define MPU_RASR_TEX 0x380000
#define MPU_RASR_SCB 0x70000
#define MPU_RASR_SRD 0xff00
#define MPU_RASR_SIZE 0x3e
#define MPU_RASR_EN 0x1

/*
 * Region size in bytes = 2^(SIZE + 1)
 * Base address is 31:N, where N = log2(Region size in bytes) = SIZE + 1
 * -> Valid address bits are ~((1u << (SIZE + 1)))
 */

struct mpu_regs {
    uint32_t TYPE;
    uint32_t CTRL;
    uint32_t RNR;
    uint32_t RBAR;
    uint32_t RASR;
    uint32_t RBAR_A1;
    uint32_t RASR_A1;
    uint32_t RBAR_A2;
    uint32_t RASR_A2;
    uint32_t RBAR_A3;
    uint32_t RASR_A3;
};

static volatile struct mpu_regs *MPU = (void *)MPU_BASE;

