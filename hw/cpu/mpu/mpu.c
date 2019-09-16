#include "mpu.h"

#define MPU_BASE 0xe000ed90

#define MPU_CTRL_PRIVDEFENA 0x4
#define MPU_CTRL_HFNMIENA 0x2
#define MPU_CTRL_ENABLE 0x1

#define MPU_RNR_REGION 0xff

#define MPU_RBAR_ADDR 0xfffffffe0
#define MPU_RBAR_VALID (1u << 4)
#define MPU_RBAR_REGION 0x7
#define MPU_RBAR_ALL (MPU_RBAR_ADDR | MPU_RBAR_VALID | MPU_RBAR_REGION)

#define MPU_RASR_ATTR 0xffff0000
#define MPU_RASR_SRD_SIZE 0xffff
#define MPU_RASR_XN (1u << 28)
#define MPU_RASR_AP 0x7000000
#define MPU_RASR_AP_SHIFT 24u
#define MPU_RASR_TEX 0x380000
#define MPU_RASR_SCB 0x70000
#define MPU_RASR_S 0x40000
#define MPU_RASR_C 0x20000
#define MPU_RASR_B 0x10000
#define MPU_RASR_SRD 0xff00
#define MPU_RASR_SRD_SHIFT 8u
#define MPU_RASR_SIZE 0x3e
#define MPU_RASR_EN 0x1
#define MPU_RASR_ALL (MPU_RASR_XN | MPU_RASR_AP | MPU_RASR_TEX | MPU_RASR_SCB \
        | MPU_RASR_SRD | MPU_RASR_SIZE | MPU_RASR_EN)

#define MPU_MIN_REGION_SIZE 4u
#define MPU_MIN_SIZE_FOR_SRD 7u
#define MPU_MAX_REGION_SIZE 31u

#define MPU_MAX_SRD 0xff
#define MPU_MAX_AP 0x7
#define MPU_MAX_TEX 0x7
#define MPU_NUM_REGIONS 8u

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

uint32_t
mpu_get_addr(const struct mpu_region *const region) {
    return (region->addr & MPU_RBAR_ADDR);
}

uint32_t
mpu_get_size(const struct mpu_region *const region) {
    return (region->attr_size & MPU_RASR_SIZE);
}

int
mpu_set_addr_size(struct mpu_region *const region, const uint32_t addr, const uint32_t size) {
    if (size < MPU_MIN_REGION_SIZE) {
        return -1;
    } else if (size > MPU_MAX_REGION_SIZE) {
        return -1;
    }

    /* Only bits [31:N] in addr are valid where N = size + 1 */
    const uint32_t unused_bits = (1u << (size + 1)) - 1;
    if (addr & unused_bits) {
        /* Bits [N-1:5] in addr should be 0 */
        return -2;
    }

    region->addr &= ~MPU_RBAR_ADDR;
    region->addr |= addr;
    region->attr_size &= ~MPU_RASR_SIZE;
    region->attr_size |= size;
    return 0;
}

uint32_t
mpu_get_srd(const struct mpu_region *const region) {
    return (region->attr_size & MPU_RASR_SRD);
}

int
mpu_set_srd(struct mpu_region *const region, const uint32_t srd) {
    if (srd > MPU_MAX_SRD) {
        return -1;
    }

    region->attr_size &= ~MPU_RASR_SRD;
    region->attr_size |= (srd << MPU_RASR_SRD_SHIFT);
    return 0;
}

uint32_t
mpu_get_attr(const struct mpu_region *const region) {
    return (region->attr_size & (MPU_RASR_XN | MPU_RASR_TEX | MPU_RASR_SCB));
}

int
mpu_set_attr(struct mpu_region *const region, const unsigned type_expansion, const unsigned executable, const unsigned cacheable, const unsigned bufferable, const unsigned shareable) {
    if (type_expansion > MPU_MAX_TEX) {
        return -1;
    }

    if (type_expansion == 0x1) {
        if ((cacheable && !bufferable) || (bufferable && !cacheable)) {
            /* Those combinations are reserved */
            return -2;
        }
    } else if (type_expansion == 0x2) {
        if ((bufferable && !cacheable) || cacheable) {
            /* Those combinations are reserved */
            return -2;
        }
    }

    region->attr_size &= ~(MPU_RASR_XN | MPU_RASR_TEX | MPU_RASR_SCB);
    region->attr_size |= type_expansion;
    if (executable) {
        region->attr_size |= MPU_RASR_XN;
    }
    if (shareable) {
        region->attr_size |= MPU_RASR_S;
    }
    if (cacheable) {
        region->attr_size |= MPU_RASR_C;
    }
    if (bufferable) {
        region->attr_size |= MPU_RASR_B;
    }

    return 0;
}

uint32_t
mpu_get_ap(const struct mpu_region *const region) {
    return (region->attr_size & MPU_RASR_AP);
}

int
mpu_set_ap(struct mpu_region *const region, const uint32_t ap) {
    if (ap > MPU_MAX_AP) {
        return -1;
    } else if (ap == MPU_AP_RSVD) {
        return -1;
    }

    region->attr_size &= ~MPU_RASR_AP;
    region->attr_size |= (ap << MPU_RASR_AP_SHIFT);
    return 0;
}

int
mpu_get_config(const unsigned num, struct mpu_region *const region) {
    if (num >= MPU_NUM_REGIONS) {
        return -1;
    }

    uint32_t rnr = MPU->RNR;
    rnr &= ~MPU_RNR_REGION;
    rnr |= num;
    MPU->RNR = rnr;
    region->addr = MPU->RBAR;
    region->attr_size = MPU->RASR;
    return 0;
}

int
mpu_set_config(const unsigned num, const struct mpu_region *const region) {
    if (num >= MPU_NUM_REGIONS) {
        return -1;
    }

    if (((region->attr_size & MPU_RASR_SIZE) < MPU_MIN_SIZE_FOR_SRD)
            && (region->attr_size & MPU_RASR_SRD)) {
        /* Subregions requires a region size of 256 B or greater */
        return -2;
    }

    const uint32_t rbar = region->addr | MPU_RBAR_VALID | num;
    MPU->RBAR = rbar;

    uint32_t rasr = MPU->RASR;
    rasr &= ~MPU_RASR_ALL;
    rasr |= region->attr_size;
    MPU->RASR = rasr;
    return 0;
}

int
mpu_region_enable(const unsigned num) {
    if (num >= MPU_NUM_REGIONS) {
        return -1;
    }

    uint32_t rnr = MPU->RNR;
    rnr &= ~MPU_RNR_REGION;
    rnr |= num;
    MPU->RNR = rnr;

    MPU->RASR |= MPU_RASR_EN;
    return 0;
}

int
mpu_region_disable(const unsigned num) {
    if (num >= MPU_NUM_REGIONS) {
        return -1;
    }

    uint32_t rnr = MPU->RNR;
    rnr &= ~MPU_RNR_REGION;
    rnr |= num;
    MPU->RNR = rnr;

    MPU->RASR &= ~MPU_RASR_EN;
    return 0;
}

void
mpu_region_init(struct mpu_region *const region) {
    region->addr = 0;
    region->attr_size = (MPU_AP_RW_RW << MPU_RASR_AP_SHIFT) | MPU_MAX_REGION_SIZE;
}

void
mpu_init(void) {
    /* Enable default mapping in privileged mode, disable MPU in Hard Faults */
    MPU->CTRL |= MPU_CTRL_PRIVDEFENA;
    MPU->CTRL &= ~MPU_CTRL_HFNMIENA;
    MPU->CTRL |= MPU_CTRL_ENABLE;
}

