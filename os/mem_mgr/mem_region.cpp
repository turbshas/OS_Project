#include "mem_region.hpp"

MemRegion::MemRegion()
    : start(0),
      size(0),
      perms(MemPermisions::None)
{
}

MemRegion::MemRegion(const uintptr_t startAddress, const size_t regionSize, MemPermisions regionPerms)
    : start(startAddress),
      size(regionSize),
      perms(regionPerms)
{
}
