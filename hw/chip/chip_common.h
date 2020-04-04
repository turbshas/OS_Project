#ifndef _CHIP_COMMON_H
#define _CHIP_COMMON_H

#include <stdio.h>

#define SRAM_BASE 0x20000000
#define SRAM_SIZE (128 * 1024)

#ifdef __STM32F4xx__
#define CCMRAM_BASE 0x10000000
#define CCMRAM_SIZE (64 * 1024)
#endif

#define FLASH_BASE 0x08000000
#define FLASH_SIZE (1024 * 1024)

#define PERIPH_BASE 0x40000000
#define PERIPH_SIZE (512 * 1024 * 1024)

#endif /* _CHIP_COMMON_H */

