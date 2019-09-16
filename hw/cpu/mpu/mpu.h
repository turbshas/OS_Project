#ifndef MPU_H
#define MPU_H

#include <stdio.h>

#define MPU_ATTR_AP 0x7000000
#define MPU_ATTR_TEX 0x380000
#define MPU_ATTR_S 0x40000
#define MPU_ATTR_C 0x20000
#define MPU_ATTR_B 0x10000

#define MPU_TEX_0 0x0
#define MPU_TEX_1 0x1
#define MPU_TEX_2 0x2
#define MPU_TEX_CACHE_MEM   0x4
/*
 * These 2 are flags to go along with MPU_TEX_CACHE_MEM.
 * When applied to type_extension, refer to outer policy.
 * When applied to cacheable and bufferable, refer to inner policy.
 *
 * MPU_TEX_W_NO_ALLOC applies to cacheable
 * MPU_TEX_WRITEBACK applies to bufferable
 */
#define MPU_TEX_W_NO_ALLOC  (1u << 1)
#define MPU_TEX_WRITEBACK   (1u << 0)

/* Format of these defines is MPU_AP_(1)_(2) where (1) is priv and (2) is unpriv */
#define MPU_AP_NONE     0x0
#define MPU_AP_RW_NONE  0x1
#define MPU_AP_RW_RO    0x2
#define MPU_AP_RW_RW    0x3
#define MPU_AP_RSVD     0x4
#define MPU_AP_RO_NONE  0x5
#define MPU_AP_RO_RO    0x6
#define MPU_AP_RO_RO2   0x7 /* Both 0x6 and 0x7 result in RO/RO */

struct mpu_region {
    uint32_t addr;
    uint32_t attr_size; /* Stores both attributes and size (same reg) */
};

/* Requirements on address and size:
 * Size must be greater than 3 and less than 32
 * The only valid bits of address are 31:(Size + 1)
 */
uint32_t mpu_get_addr(const struct mpu_region *const region);
uint32_t mpu_get_size(const struct mpu_region *const region);
int mpu_set_addr_size(struct mpu_region *const region, const uint32_t addr, const uint32_t size);
/*
 * SRD is only valid for regions of size >= 128 B, this will be
 * checked while setting the config.
 *
 * SRD is a bitmap - one bit for each of the 8 subregions starting from bit 0.
 */
uint32_t mpu_get_srd(const struct mpu_region *const region);
int mpu_set_srd(struct mpu_region *const region, const uint32_t srd);
/*
 * Valid combinations: (x = don't care, b = determines shareable/unshareable)
 * TEX      C B S
 * 0b000    0 0 x Strongly-ordered, shareable
 * 0b000    0 1 x Device, shareable
 * 0b000    1 0 b Normal, write-through, write-no-allocate
 * 0b000    1 1 b Normal, write-back, write-no-allocate
 * 0b001    0 0 b Normal, noncacheable
 * 0b001    0 1 x Reserved
 * 0b001    1 0 x Implementation-specific
 * 0b001    1 1 b Noraml, write-back, write-allocate
 * 0b010    0 0 x Device, unshareable
 * 0b010    0 1 x Reserved
 * 0b010    1 x x Reserved
 * 0b1BB    A A b Normal, cached memory, BB = outer policy, AA = inner policy
 *     First A/B = write-no-allocate, second A/B = write-back
 *
 * Default regions:
 * Region Name      Start       End         XN  Cache   C B Memory Type
 * Code             0x00000000  0x1fffffff  -   WT      Y - Normal
 * SRAM             0x20000000  0x3fffffff  -   WB-WA   Y Y Normal
 * Peripheral       0x40000000  0x5fffffff  Y   -       - Y Device
 * RAM              0x60000000  0x7fffffff  -   WB-WA   Y Y Normal
 * RAM              0x80000000  0x9fffffff  -   WT      Y Y Normal
 * Devices          0xA0000000  0xbfffffff  Y   -       - Y Device
 * Devices          0xC0000000  0xdfffffff  Y   -       - Y Device
 * System - PPB     0xE0000000  0xE00fffff  Y   -       - - Strongly Ordered
 * System - Vendor  0xE0100000  0xffffffff  Y   -       - Y Device
 */
uint32_t mpu_get_attr(const struct mpu_region *const region);
int mpu_set_attr(struct mpu_region *const region, const unsigned type_expansion, const unsigned executable, const unsigned cacheable, const unsigned bufferable, const unsigned shareable);
uint32_t mpu_get_ap(const struct mpu_region *const region);
int mpu_set_ap(struct mpu_region *const region, const uint32_t ap);

/* When setting the config, it will first be checked for validity */
void mpu_region_init(struct mpu_region *const region);
int mpu_get_config(const unsigned num, struct mpu_region *const region);
int mpu_set_config(const unsigned num, const struct mpu_region *const region);
int mpu_region_enable(const unsigned num);
int mpu_region_disable(const unsigned num);

/* Notes:
 *  - Enable MPU
 *  - Disable MPU during faults
 *  - Enable default region
 */
void mpu_init(void);

#endif /* MPU_H */

