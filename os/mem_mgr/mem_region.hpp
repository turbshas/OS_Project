#ifndef _MEM_REGION_H
#define _MEM_REGION_H

#include <cstddef>
#include <cstdint>

/// @brief Usage permissions for a given region of memory, as a bit-field.
enum class MemPermisions : uint8_t
{
    None = 0x0,
    Read = 0x1,
    Write = 0x2,
    Execute = 0x4,
};

/// @brief Represents a region in memory to be used by a process.
class MemRegion
{
    public:
        /// @brief Start address of the memory region.
        const uintptr_t start;
        /// @brief Size of the memory region, in bytes.
        const size_t size;
        /// @brief Allowed access permissions for the memory region.
        MemPermisions perms;

        // MemRegion();
        MemRegion(const uintptr_t startAddress, const size_t regionSize, MemPermisions permissions);
        /// @brief Get the end address of the last byte in the memory region.
        uintptr_t getEnd() const { return start + size - 1; };
};

#endif /* _MEM_REGION_H */
