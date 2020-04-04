#include "mpu_region.h"

#define MPU_MIN_REGION_SIZE 4u
#define MPU_MAX_REGION_SIZE 31u

#define MPU_MAX_SRD 0xff
#define MPU_MAX_AP 0x7
#define MPU_MAX_TEX 0x7

mpu_region::mpu_region()
{
    addr = 0;
    size = 3;
    srd = 0;
    te = static_cast<enum type_expansions>(TEX_CACHED_MEM | TEX_1);
    executable = false;
    cacheable = false;
    bufferable = true;
    shareable = false;
    ap = AP_NONE;
}

int
mpu_region::set_addr_size(const uint32_t addr_val, const uint32_t size_val)
{
    if (size_val < MPU_MIN_REGION_SIZE) {
        return -1;
    } else if (size_val > MPU_MAX_REGION_SIZE) {
        return -1;
    }

    /* Only bits [31:N] in addr are valid where N = size + 1 */
    const uint32_t unused_bits = (1u << (size_val + 1)) - 1;
    if (addr_val & unused_bits) {
        /* Bits [N-1:5] in addr should be 0 */
        return -2;
    }

    addr = addr_val;
    size = size_val;
    return 0;
}

int
mpu_region::set_subregion_disable_bits(const uint32_t srd_val)
{
    if (srd_val > MPU_MAX_SRD) {
        return -1;
    }

    srd = srd_val;
    return 0;
}

int
mpu_region::set_attr(const enum type_expansions type_expansion, const bool executable_val, const bool cacheable_val, const bool bufferable_val, const bool shareable_val)
{
    if (type_expansion > MPU_MAX_TEX) {
        return -1;
    }

    if (type_expansion == TEX_1) {
        if ((cacheable_val && !bufferable_val) || (bufferable_val && !cacheable_val)) {
            /* Those combinations are reserved */
            return -2;
        }
    } else if (type_expansion == TEX_2) {
        if ((bufferable_val && !cacheable_val) || cacheable_val) {
            /* Those combinations are reserved */
            return -2;
        }
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
    if (ap_val > MPU_MAX_AP) {
        return -1;
    } else if (ap_val == AP_RSVD) {
        return -1;
    }

    ap = ap_val;
    return 0;
}

