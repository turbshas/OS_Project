#ifndef MEM_MGR_H
#define MEM_MGR_H

#include <cstdio>
#include "pageList.h"
#include "process.h"

#define PAGE_SIZE (2 * 1024)

class MemoryManager {
    PageList _pageList;

    public:
        MemoryManager();
        void Initialize();
        void AllocatePages(Process *const process, const size_t size);
};

extern MemoryManager memoryManager;

#endif /* MEM_MGR_H */

