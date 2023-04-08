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

MemoryManager memoryManager;

MemoryManager::MemoryManager()
    : _pageList()
{
}

void
MemoryManager::Initialize()
{
    const uintptr_t dataRamStart = reinterpret_cast<uintptr_t>(&_DATA_RAM_START);
    const uintptr_t allocableMem = reinterpret_cast<uintptr_t>(&_ALLOCABLE_MEM);
    // All memory not used by static data.
    const size_t totalAllocableMem = dataRamStart + SRAM_SIZE - allocableMem;
    // Only care about whole pages, so round down to nearest multiple of page size.
    const size_t allocablePages = totalAllocableMem / PAGE_SIZE;
    // Allocation needs to start at an aligned address - round up to nearest page boundary
    const uintptr_t alignedAllocationStart =
        (
            (allocableMem - 1) // When the value is already a multiple of PAGE_SIZE, this prevents adding an extra PAGE_SIZE amount
            & ~(PAGE_SIZE - 1) // Rounds down to aligned amount
        )
        + PAGE_SIZE; // Add 1 back to recover the missed amount
    
    // Memory will be reserved for the kernel when its process is initialized.
    // TODO: initial stack (kernel stack) needs to be un-allocable
    const size_t allocableMemSize = allocablePages * PAGE_SIZE;
    void *const allocationStart = reinterpret_cast<void *>(alignedAllocationStart);
    _pageList.freePages(allocablePages, allocationStart);

    MPU->init();
}

void
MemoryManager::AllocatePages(Process *const process, const size_t size) {
    const size_t roundedDown = (size - 1) & ~(PAGE_SIZE - 1);
    const size_t roundedUp = roundedDown + PAGE_SIZE;
    const size_t numPages = roundedUp / PAGE_SIZE;
    return _pageList.allocatePages(numPages);
}
