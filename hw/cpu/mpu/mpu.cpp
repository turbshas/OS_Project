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
#define MPU_RASR_TEX_SHIFT 19u
#define MPU_RASR_SCB 0x70000
#define MPU_RASR_S 0x40000
#define MPU_RASR_C 0x20000
#define MPU_RASR_B 0x10000
#define MPU_RASR_SRD 0xff00
#define MPU_RASR_SRD_SHIFT 8u
#define MPU_RASR_SIZE 0x3e
#define MPU_RASR_SIZE_SHIFT 1u
#define MPU_RASR_EN 0x1
#define MPU_RASR_ALL (MPU_RASR_XN | MPU_RASR_AP | MPU_RASR_TEX | MPU_RASR_SCB \
        | MPU_RASR_SRD | MPU_RASR_SIZE | MPU_RASR_EN)

#define MPU_MIN_SIZE_FOR_SRD 7u
#define MPU_NUM_REGIONS 8u

/*
 * Region size in bytes = 2^(SIZE + 1)
 * Base address is 31:N, where N = log2(Region size in bytes) = SIZE + 1
 * -> Valid address bits are ~((1u << (SIZE + 1)))
 */

volatile Mpu *const MPU = reinterpret_cast<volatile Mpu *>(MPU_BASE);

uint32_t
Mpu::get_rbar(const unsigned num, const mpu_region &region) const volatile
{
    return region.get_addr() | MPU_RBAR_VALID | num;
}

uint32_t
Mpu::get_rasr(const mpu_region &region) const volatile
{
    uint32_t rasr = region.get_access_perms() << MPU_RASR_AP_SHIFT
        | region.get_type_expansion() << MPU_RASR_TEX_SHIFT
        | region.get_subregion_disable_bits() << MPU_RASR_SRD_SHIFT
        | region.get_size() << MPU_RASR_SIZE_SHIFT;

    if (!region.get_executable()) {
        rasr |= MPU_RASR_XN;
    }
    if (region.get_cacheable()) {
        rasr |= MPU_RASR_C;
    }
    if (region.get_bufferable()) {
        rasr |= MPU_RASR_B;
    }
    if (region.get_shareable()) {
        rasr |= MPU_RASR_S;
    }

    return rasr;
}

int
Mpu::get_config(const unsigned num, mpu_region &region) volatile
{
    if (num >= MPU_NUM_REGIONS) {
        return -1;
    }

    uint32_t rnr = RNR;
    rnr &= ~MPU_RNR_REGION;
    rnr |= num;
    RNR = rnr;

    const uint32_t rbar_val = RBAR;
    const uint32_t rasr_val = RASR;

    const uint32_t addr = rbar_val & MPU_RBAR_ADDR;
    const uint32_t size = rasr_val & MPU_RASR_SIZE;
    region.set_addr_size(addr, size);

    const uint32_t srd = (rasr_val & MPU_RASR_SRD) >> MPU_RASR_SRD_SHIFT;
    region.set_subregion_disable_bits(srd);

    const enum mpu_region::access_permissions ap = static_cast<enum mpu_region::access_permissions>((rasr_val & MPU_RASR_AP) >> MPU_RASR_AP_SHIFT);
    region.set_access_perms(ap);

    const enum mpu_region::type_expansions te = static_cast<enum mpu_region::type_expansions>((rasr_val & MPU_RASR_TEX) >> MPU_RASR_TEX_SHIFT);
    const bool xn = !(rasr_val & MPU_RASR_XN);
    const bool c = rasr_val & MPU_RASR_C;
    const bool b = rasr_val & MPU_RASR_B;
    const bool s = rasr_val & MPU_RASR_S;
    region.set_attr(te, xn, c, b, s);

    return 0;
}

int
Mpu::set_config(const unsigned num, const mpu_region &region) volatile
{
    if (num >= MPU_NUM_REGIONS) {
        return -1;
    }

    if ((region.get_size() < MPU_MIN_SIZE_FOR_SRD)
            && (region.get_subregion_disable_bits() != 0)) {
        /* Subregions requires a region size of 256 B or greater */
        return -2;
    }

    const uint32_t rbar_val = get_rbar(num, region);
    const uint32_t rasr_val = get_rasr(region);

    uint32_t rasr = RASR;
    rasr &= ~MPU_RASR_ALL;
    rasr |= rasr_val;

    RBAR = rbar_val;
    RASR = rasr;
    return 0;
}

int
Mpu::region_enable(const unsigned num) volatile
{
    if (num >= MPU_NUM_REGIONS) {
        return -1;
    }

    uint32_t rnr = RNR;
    rnr &= ~MPU_RNR_REGION;
    rnr |= num;
    RNR = rnr;

    RASR |= MPU_RASR_EN;
    return 0;
}

int
Mpu::region_disable(const unsigned num) volatile
{
    if (num >= MPU_NUM_REGIONS) {
        return -1;
    }

    uint32_t rnr = RNR;
    rnr &= ~MPU_RNR_REGION;
    rnr |= num;
    RNR = rnr;

    RASR &= ~MPU_RASR_EN;
    return 0;
}

void
Mpu::init(void) volatile
{
    /* Enable default mapping in privileged mode, disable MPU in Hard Faults */
    CTRL |= MPU_CTRL_PRIVDEFENA;
    CTRL &= ~MPU_CTRL_HFNMIENA;
    CTRL |= MPU_CTRL_ENABLE;
}

