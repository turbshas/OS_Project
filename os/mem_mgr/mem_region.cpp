#include "mem_region.hpp"

MemRegion::MemRegion()
    : start(), size(), perms()
{
}

MemRegion::MemRegion(uintptr_t startAddr, size_t regionSize, MemPermisions regionPerms)
    : start(startAddr), size(regionSize), perms(regionPerms)
{
}
