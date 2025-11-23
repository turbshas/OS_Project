#include "mpu_region.h"

#define MPU_MIN_REGION_SIZE 4u
#define MPU_MAX_REGION_SIZE 31u

#define MPU_MAX_SRD 0xff
#define MPU_MAX_AP 0x7
#define MPU_MAX_TEX 0x7

mpu_region::mpu_region()
    : addr(0),
    size(3),
    srd(0),
    te(static_cast<enum type_expansions>(TEX_CACHED_MEM | TEX_1)),
    executable(false),
    cacheable(false),
    bufferable(true),
    shareable(false),
    ap(AP_NONE)
{
}

bool
mpu_region::check_size(const uint32_t size_val) const
{
    if (size_val < MPU_MIN_REGION_SIZE) {
        return false;
    } else if (size_val > MPU_MAX_REGION_SIZE) {
        return false;
    }
    return true;
}

bool
mpu_region::check_addr(const uint32_t size_val, const uint32_t addr_val) const
{
    /* Only bits [31:N] in addr are valid where N = size + 1 */
    const uint32_t unused_bits = (1u << (size_val + 1)) - 1;
    if (addr_val & unused_bits) {
        return false;
    }
    return true;
}

bool
mpu_region::check_subregion_disable(const uint32_t srd_val) const
{
    if (srd_val > MPU_MAX_SRD) {
        return false;
    }
    return true;
}

bool
mpu_region::check_attributes(const enum type_expansions type_expansion, const bool cacheable_val, const bool bufferable_val) const
{
    if (type_expansion > MPU_MAX_TEX) {
        return false;
    }

    if (type_expansion == TEX_1) {
        if ((cacheable_val && !bufferable_val) || (bufferable_val && !cacheable_val)) {
            /* Those combinations are reserved */
            return false;
        }
    } else if (type_expansion == TEX_2) {
        if ((bufferable_val && !cacheable_val) || cacheable_val) {
            /* Those combinations are reserved */
            return false;
        }
    }
    return true;
}

bool
mpu_region::check_permissions(const enum access_permissions ap_val) const
{
    if (ap_val > MPU_MAX_AP) {
        return false;
    } else if (ap_val == AP_RSVD) {
        return false;
    }
    return true;
}

bool
mpu_region::check_valid() const
{
    if (!check_size(size)) return false;
    if (!check_addr(size, addr)) return false;
    if (!check_subregion_disable(srd)) return false;
    if (!check_attributes(te, cacheable, bufferable)) return false;
    if (!check_permissions(ap)) return false;
    return true;
}

int
mpu_region::set_addr_size(const uint32_t addr_val, const uint32_t size_val)
{
    if (!check_size(size_val)) {
        return -1;
    }

    if (!check_addr(size_val, addr_val)) {
        return -2;
    }

    addr = addr_val;
    size = size_val;
    return 0;
}

int
mpu_region::set_subregion_disable_bits(const uint32_t srd_val)
{
    if (!check_subregion_disable(srd_val)) {
        return -1;
    }

    srd = srd_val;
    return 0;
}

int
mpu_region::set_attr(const enum type_expansions type_expansion, const bool executable_val, const bool cacheable_val, const bool bufferable_val, const bool shareable_val)
{
    if (!check_attributes(type_expansion, cacheable_val, bufferable_val)) {
        return -1;
    }

    te = type_expansion;
    executable = executable_val;
    cacheable = cacheable_val;
    bufferable = bufferable_val;
    shareable = shareable_val;

    return 0;
}

int
mpu_region::set_access_perms(const enum access_permissions ap_val)
{
    if (!check_permissions(ap_val)) {
        return -1;
    }

    ap = ap_val;
    return 0;
}

