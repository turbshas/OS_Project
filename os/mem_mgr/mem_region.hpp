#ifndef _MEM_REGION_H
#define _MEM_REGION_H

#include <cstdint>

/// @brief Usage permissions for a given region of memory, as a bit-field.
enum class MemPermisions : uint8_t {
    Read = 0x1,
    Write = 0x2,
    Execute = 0x4,
};

/// @brief Represents a region in memory to be used by a process.
struct MemRegion {
    uintptr_t start;
    size_t size;
    MemPermisions perms;

    MemRegion();
    MemRegion(uintptr_t, size_t, MemPermisions);
    uintptr_t getEnd() const { return start + size; };
};

#endif _MEM_REGION_H
