#include "chip_common.h"
#include "mem_mgr.h"

/* What mem_mgr needs to do:
 *   - Allocate chunks of memory for the malloc implementation
 *     - E.g. 4k pages at a time
 *     - Memory used for dynamic structures in the kernel
 *     - Memory given to processes
 *   - Manage the MPU/MMU to give processes access to only their section of memory
 *     - When the process is given its initial page, set the first MPU region to that
 *     - As pages are added, extend the region (if possible) or add more regions
 */

/* Variables defined in the linker script */
extern unsigned int _ALLOCABLE_MEM;
extern unsigned int _DATA_RAM_START;

static void *const ALLOCABLE_MEM_START = &_ALLOCABLE_MEM;
static size_t ALLOCABLE_MEM_SIZE;

void
mem_mgr_init()
{
    const uintptr_t p_drs_int = reinterpret_cast<uintptr_t>(&_DATA_RAM_START);
    const uintptr_t p_am_int = reinterpret_cast<uintptr_t>(&_ALLOCABLE_MEM);
    ALLOCABLE_MEM_SIZE = p_drs_int + SRAM_SIZE - p_am_int;
}

