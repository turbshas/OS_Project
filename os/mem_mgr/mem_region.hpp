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
    private:
        /// @brief Start address of the memory region.
        uintptr_t _start;
        /// @brief Size of the memory region, in bytes.
        size_t _size;
        /// @brief Allowed access permissions for the memory region.
        MemPermisions _perms;

    public:
        MemRegion();
        MemRegion(const uintptr_t startAddress, const size_t regionSize, MemPermisions permissions);
        MemRegion(const MemRegion&) = default;
        MemRegion(MemRegion&&) = default;
        ~MemRegion() = default;
        MemRegion& operator=(const MemRegion&);
        MemRegion& operator=(MemRegion&&);

        /// @brief Get the end address of the last byte in the memory region.
        uintptr_t start() const { return _start; };
        size_t size() const { return _size; };
        uintptr_t getEnd() const { return _start + _size - 1; };
        MemPermisions perms() const { return _perms; };
};

#endif /* _MEM_REGION_H */
