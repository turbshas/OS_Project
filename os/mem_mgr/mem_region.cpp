#include "mem_region.hpp"

// MemRegion::MemRegion()
//     : start(),
//       size(),
//       perms()
// {
// }

MemRegion::MemRegion(const uintptr_t startAddress, const size_t regionSize, MemPermisions regionPerms)
    : start(startAddress),
      size(regionSize),
      perms(regionPerms)
{
}
