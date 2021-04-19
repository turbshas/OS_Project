#include "chip_common.h"
#include "mem_mgr.h"
#include "mpu.h"

/* What mem_mgr needs to do:
 *   - Allocate chunks of memory for the malloc implementation
 *     - E.g. 4k pages at a time
 *     - Memory used for dynamic structures in the kernel
 *     - Memory given to processes
 *   - Manage the MPU/MMU to give processes access to only their section of memory
 *     - When the process is given its initial page, set the first MPU region to that
 *     - As pages are added, extend the region (if possible) or add more regions
 * 
 * Need to define the actual regions of memory available to mem_mgr
 *   - ALLOCABLE_MEM_START is after ROM and BSS sections, where the heap section starts
 *     On Cortex-M, the SRAM region starts at 0x20000000 which also happens to be the
 *     bit-band region (1 MB) followed by 31 MB of normal SRAM followed by 32 MB of
 *     the bit-band's alias section. Don't write into this alias section to avoid
 *     clobbering the data in the bit-band region
 */

/* Variables defined in the linker script */
extern unsigned int _ALLOCABLE_MEM;
extern unsigned int _DATA_RAM_START;

static void *const ALLOCABLE_MEM_START = &_ALLOCABLE_MEM;
static size_t NUM_ALLOCABLE_PAGES;
static size_t ALLOCABLE_MEM_SIZE;
static void *ALLOCATION_START;

class PageList {
    // Will be casting the start of each sequence to a pointer to one of these
    struct PageSequence {
        uint32_t numPages;
        struct PageSequence *prev;
        struct PageSequence *next;

        void insertAfter(PageSequence& insert)
        {
            insert.next = next;
            insert.prev = this;
            insert.next->prev = &insert;
            next = &insert;
        }

        void insertBefore(PageSequence& insert)
        {
            insert.next = this;
            insert.prev = prev;
            insert.prev->next = &insert;
            prev = &insert;
        }

        struct PageSequence *remove()
        {
            prev->next = next;
            next->prev = prev;
            next = nullptr;
            prev = nullptr;
            return this;
        }
    };

    struct PageSequence sentinel;

    bool areSequencesAdjacent(const PageSequence& first, const PageSequence& second) const;

    public:
        PageList();
        void initialize(const size_t numPages, void *const startAddr);
        void *allocatePages(const size_t numPages);
        void freePages(const size_t numPages, void *startAddr);
};

PageList::PageList()
{
    sentinel.numPages = 0;
    sentinel.next = &sentinel;
    sentinel.prev = &sentinel;
}

void
PageList::initialize(const size_t numPages, void *const startAddr)
{
    PageSequence *const initialSequence = static_cast<PageSequence *>(startAddr);
    initialSequence->numPages = numPages;
    initialSequence->next = &sentinel;
    initialSequence->prev = &sentinel;

    sentinel.next = initialSequence;
    sentinel.prev = initialSequence;
}

bool
PageList::areSequencesAdjacent(const PageSequence& first, const PageSequence& second) const
{
    const uintptr_t firstAddr = reinterpret_cast<uintptr_t>(&first);
    const uintptr_t secondAddr = reinterpret_cast<uintptr_t>(&second);

    if (firstAddr == secondAddr) {
        return true;
    }

    if (firstAddr < secondAddr) {
        const size_t firstSize = first.numPages * PAGE_SIZE;
        return (firstAddr + firstSize) == secondAddr;
    } else {
        const size_t secondSize = second.numPages * PAGE_SIZE;
        return (secondAddr + secondSize) == firstAddr;
    }
}

void *
PageList::allocatePages(const size_t numPages)
{
    if (sentinel.next == &sentinel) {
        /* We have allocated every page */
        return nullptr;
    }

    PageSequence *iterator = sentinel.next;
    while (iterator != &sentinel) {
        if (iterator->numPages >= numPages) {
            break;
        }
        iterator = iterator->next;
    }

    if (iterator == &sentinel) {
        // Found no suitable sequence of pages
        return nullptr;
    }

    PageSequence *const prevSequence = iterator->prev;
    iterator->remove();
    if (iterator->numPages > numPages) {
        // Need to put the extra pages back in the list
        const uintptr_t iterator_int = reinterpret_cast<uintptr_t>(iterator);
        const size_t leftoverPages = iterator->numPages - numPages;
        const uintptr_t reinsertSequenceAddr = iterator_int + (leftoverPages * PAGE_SIZE);

        PageSequence *const pagesToReinsert = reinterpret_cast<PageSequence *>(reinsertSequenceAddr);
        pagesToReinsert->numPages = leftoverPages;
        prevSequence->insertAfter(*pagesToReinsert);
    }

    return static_cast<void *>(iterator);
}

void
PageList::freePages(const size_t numPages, void *startAddr)
{
    PageSequence *const freedSequence = static_cast<PageSequence *>(startAddr);
    freedSequence->numPages = numPages;
    freedSequence->next = nullptr;
    freedSequence->prev = nullptr;

    if (sentinel.next == sentinel.prev) {
        // List is empty
        sentinel.insertAfter(*freedSequence);
        return;
    }

    // Find the block after the freed one
    PageSequence *iterator = sentinel.next;
    while ((iterator != &sentinel) && (iterator < freedSequence)) {
        iterator = iterator->next;
    }

    iterator->insertBefore(*freedSequence);

    // Check if we can coalesce blocks
    PageSequence *const followingSequence = freedSequence->next;
    if (areSequencesAdjacent(*freedSequence, *followingSequence)) {
        // Can coalesce freed block with following block
        freedSequence->numPages += followingSequence->numPages;
        freedSequence->next = followingSequence->next;
        freedSequence->next->prev = freedSequence;
    }

    PageSequence *const precedingSequence = freedSequence->prev;
    if (areSequencesAdjacent(*freedSequence, *precedingSequence)) {
        // Can coalesce freed block with preceding one
        precedingSequence->numPages += freedSequence->numPages;
        precedingSequence->next = freedSequence->next;
        precedingSequence->next->prev = precedingSequence;
    }
}

static PageList pageList;

void *allocatePages(const size_t size) {
    const size_t roundedDown = (size - 1) & ~(PAGE_SIZE - 1);
    const size_t roundedUp = roundedDown + PAGE_SIZE;
    const size_t numPages = roundedUp / PAGE_SIZE;
    return pageList.allocatePages(numPages);
}

void
mem_mgr_init()
{
    const uintptr_t dataRamStart = reinterpret_cast<uintptr_t>(&_DATA_RAM_START);
    const uintptr_t allocableMem = reinterpret_cast<uintptr_t>(&_ALLOCABLE_MEM);
    const size_t allocableMemSize = dataRamStart + SRAM_SIZE - allocableMem;
    
    // Number of whole pages - ignore the extra
    NUM_ALLOCABLE_PAGES = allocableMemSize / PAGE_SIZE;
    ALLOCABLE_MEM_SIZE = NUM_ALLOCABLE_PAGES * PAGE_SIZE;
    // Allocation needs to start at an aligned address
    const uintptr_t alignedAllocationStart = ((allocableMem - 1) & ~(PAGE_SIZE - 1)) + PAGE_SIZE;
    ALLOCATION_START = reinterpret_cast<void *>(alignedAllocationStart);
    pageList.initialize(NUM_ALLOCABLE_PAGES, ALLOCATION_START);

    MPU->init();
}

