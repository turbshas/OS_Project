#include "mem_region.hpp"

MemRegion::MemRegion()
    : _start(0),
      _size(0),
      _perms(MemPermisions::None)
{
}

MemRegion::MemRegion(const uintptr_t startAddress, const size_t regionSize, MemPermisions regionPerms)
    : _start(startAddress),
      _size(regionSize),
      _perms(regionPerms)
{
}

MemRegion&
MemRegion::operator=(const MemRegion& other)
{
    if (this == &other) return *this;

    _start = other._start;
    _size = other._size;
    _perms = other._perms;

    return *this;
}

MemRegion&
MemRegion::operator=(MemRegion&& other)
{
    if (this == &other) return *this;

    _start = other._start;
    other._start = 0;

    _size = other._size;
    other._size = 0;

    _perms = other._perms;
    other._perms = MemPermisions::None;

    return *this;
}
