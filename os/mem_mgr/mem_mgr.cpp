#include "chip_common.h"
#include "mem_mgr.h"
#include "mpu.h"
#include "pageList.h"

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
    
    // Number of whole pages - ignore the extra. Reserve 1 page for kernel stack (at end of mem region)
    NUM_ALLOCABLE_PAGES = (allocableMemSize / PAGE_SIZE) - 1;
    ALLOCABLE_MEM_SIZE = NUM_ALLOCABLE_PAGES * PAGE_SIZE;
    // Allocation needs to start at an aligned address - round up to nearest page boundary
    const uintptr_t alignedAllocationStart = ((allocableMem - 1) & ~(PAGE_SIZE - 1)) + PAGE_SIZE;
    ALLOCATION_START = reinterpret_cast<void *>(alignedAllocationStart);
    pageList.initialize(NUM_ALLOCABLE_PAGES, ALLOCATION_START);

    MPU->init();
}

