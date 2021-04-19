#ifndef MEM_MGR_H
#define MEM_MGR_H

#include <cstdio>

#define PAGE_SIZE (2 * 1024)

void *allocatePages(const size_t size);
void mem_mgr_init();

#endif /* MEM_MGR_H */

