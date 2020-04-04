#ifndef _MPU_REGION_H
#define _MPU_REGION_H

#include <cstdio>

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

class mpu_region {
    public:
        enum type_expansions {
            TEX_0 = 0x0, TEX_1 = 0x1, TEX_2 = 0x2, TEX_CACHED_MEM = 0x4
        };
        /* Format of these is MPU_AP_(1)_(2) where (1) is priv and (2) is unpriv */
        enum access_permissions { AP_NONE = 0, AP_RW_NONE, AP_RW_RO, AP_RW_RW,
                                  AP_RSVD, AP_RO_NONE, AP_RO_RO, AP_RO_RO2 };
        mpu_region();
        /* Requirements on address and size:
         * Size must be greater than 3 and less than 32
         * The only valid bits of address are 31:(Size + 1)
         */
        uint32_t get_addr() const { return addr; };
        uint32_t get_size() const { return size; };
        int set_addr_size(const uint32_t addr_val, const uint32_t size_val);
        /*
         * SRD is only valid for regions of size >= 128 B, this will be
         * checked while setting the config.
         *
         * SRD is a bitmap - one bit for each of the 8 subregions starting from bit 0.
         */
        uint32_t get_subregion_disable_bits() const { return srd; };
        int set_subregion_disable_bits(const uint32_t srd_val);
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
        enum type_expansions get_type_expansion() const { return te; };
        bool get_executable() const { return executable; };
        bool get_cacheable() const { return cacheable; };
        bool get_bufferable() const { return bufferable; };
        bool get_shareable() const { return shareable; };
        int set_attr(const enum type_expansions type_expansion, const bool executable, const bool cacheable, const bool bufferable, const bool shareable);

        uint32_t get_access_perms() const { return ap; };
        int set_access_perms(const enum access_permissions ap_val);

    private:
        uint32_t addr;
        uint32_t size;
        uint32_t srd;
        enum type_expansions te;
        bool executable;
        bool cacheable;
        bool bufferable;
        bool shareable;
        enum access_permissions ap;
};

#endif /* _MPU_REGION_H */

