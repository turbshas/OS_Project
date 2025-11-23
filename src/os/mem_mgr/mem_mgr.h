#ifndef MEM_MGR_H
#define MEM_MGR_H

#include "mem_region.hpp"
#include "pageList.h"
#include <cstdint>

#define PAGE_SIZE (2 * 1024)

class MemoryManager
{
    private:
        PageList _pageList;

    public:
        MemoryManager();
        MemoryManager(const MemoryManager&) = delete;
        MemoryManager(MemoryManager&&) = delete;
        ~MemoryManager();
        MemoryManager& operator=(const MemoryManager&) = delete;
        MemoryManager& operator=(MemoryManager&&) = delete;

        void Initialize();
        const MemRegion Allocate(const size_t numBytes);
        void Free(const MemRegion& memRegion);
};

extern MemoryManager memoryManager;

#endif /* MEM_MGR_H */
