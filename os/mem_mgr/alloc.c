#include "mem_mgr.h"
/*
 * For memoy allocation, a skip list will be used with 4 different free lists:
 * 0 B, 16 B, 64 B, and 1024 B
 *
 * A block of memory greater than or equal to the size of the ith list will be
 * placed on that list and will point to the next item in that list. Each list
 * will be singly-linked and only contain free blocks of memory.
 *
 * For example: (* denotes NULL)
 *          ______     _______     ______     ______     ______     ______
 *    0 -->|      |-->|       |-->|      |-->|      |-->|      |-->|   *  |
 *         | 1024 |   |    8  |   |  32  |   |  128 |   | 4096 |   |  40  |
 *   16 -->|      |---|       |-->|      |-->|      |-->|      |-->|   *  |
 *         |      |   |       |   |      |   |      |   |      |   |      |
 *   64 -->|      |---|       |---|      |-->|      |-->|   *  |   |      |
 *         |      |   |       |   |      |   |      |   |      |   |      |
 * 1024 -->|      |---|       |---|      |---|      |-->|   *  |   |      |
 *         |______|   |_______|   |______|   |______|   |______|   |______|
 */

#define SRAM_SIZE (128 * 1024)
#define NUM_FREE_LISTS 4u
#define MIN_ALLOC_SIZE 8u

extern unsigned int _ALLOCABLE_MEM;
extern unsigned int _DATA_RAM_START;

/*
 * This struct will be placed at the beginning of each free entry. Thus, it will
 * never be allocated explicitly. A pointer to the beginning of the entry will
 * be cast to a pointer this header.
 *
 * Therefore, the size of the "next" array will depend on the size of the entry
 * itself. The array is declared as being a length of NUM_FREE_LISTS to supress
 * compiler warnings.
 */
struct free_entry {
    size_t size;
    struct free_entry *next[NUM_FREE_LISTS];
};

struct list_links {
    /* Each pointer points to a next value of a free_entry element */
    struct free_entry **lists[NUM_FREE_LISTS];
};

/* Start of allocable memory block (and size), maybe make this a macro? */
static void *const ALLOCABLE_MEM_START = &_ALLOCABLE_MEM;
static size_t ALLOCABLE_MEM_SIZE;

/* Entry point for each skip list */
static struct free_entry free_list_start;

/*
 * Planned interface:
 *  1) ker_malloc:
 *    - params: size of memory
 *    - traverse free list until past start
 *    - find first free entry of sufficient size to allocate request
 *    - if exact match or leftover would be too small: allocate entire entry
 *    - else fragment entry, allocate first chunk
 *    - previous entries need to be updated to point to correct next entry
 *      - use 2 list_links objects, 1 behind the other
 *  2) ker_calloc:
 *    - params: size of memory
 *    - calls ker_malloc, zeroes out memory
 *  3) ker_realloc:
 *    - params: old size of memory, new size of memory, old pointer
 *    - if there is a block directly next to old one (and it fits), expand it
 *    - else, allocate new block, copy data over, and free old block
 *  4) ker_free:
 *    - params: size, pointer
 *    - find spot where block should go in free list
 *    - if contiguous with next block, merge with next block
 *    - if contiguous with previous block, merge with previous
 *    - else, make it a new block and insert into list, update pointers
 *
 * Notes:
 *  - Need minimum alloc size (size of size_t + size of void *) - in this case, 8 B
 *  - For processes allocating memory, may need functions that allocate requested
 *    size + header size, and the header stores the size of the memory allocated
 *    for convenient freeing by process (free would only need the pointer)
 *  - May need similar thing for realloc - inner function accepts old memory size
 *    as well as new memory size
 *  - Start location of allocable memory will be communicated by the linker script
 *    (by communicating end of memory used in the OS binary) and size will be
 *    computed from this
 * TODO: make sure allocated blocks are multiples of 4 B for alignment and also
 * that freed pointers are aligned correctly (and realloc'd ones too)
 */

//TODO: can do this with a lookup table?
static unsigned
which_skiplist_by_size(const size_t size) {
    if (size >= 1024) {
        return 3;
    } else if (size >= 64) {
        return 2;
    } else if (size >= 16) {
        return 1;
    } else {
        return 0;
    }
}

static void
advance_in_list(const unsigned skip_list, const struct free_entry **const p, struct list_links *const links) {
    *p = (*p)->next[skip_list];
    for (unsigned i = 0; i < NUM_FREE_LISTS; i++) {
        if (*(links->lists[i]) < (*p)) {
            /* Advance the links forward, but only if they don't pass p.
             * This is because the links will be used to update the next
             * pointers in the list once an entry is allocated, so we need
             * to stay behind p.
             */
            struct free_entry *next_entry = *(links->lists[i]);
            links->lists[i] = &next_entry->next[i];
        }
    }
}

void *
ker_malloc(const size_t size) {
    if (size == 0) {
        return NULL;
    }

    const unsigned skip_list = which_skiplist_by_size(size);

    struct free_entry *p = free_list_start.next[skip_list];
    struct list_links links;
    for (unsigned i = 0; i < NUM_FREE_LISTS; i++) {
        links.lists[i] = &free_list_start.next[i];
    }

    void *ret = NULL;
    while (p) {
        if (p->size >= size) {
            /* We can use this block of memory */
            if (p->size < (size + MIN_ALLOC_SIZE)) {
                /*
                 * If we allocated this block, the leftover would
                 * be too small. Allocate the whole thing
                 */
                ret = p;

                /* Update pointers that pointed at p to point to the block after p */
                for (unsigned i = 0; i <= skip_list; i++) {
                    *(links.lists[i]) = p->next[i];
                }
                break;
            } else {
                /* We need to split the block */
                ret = p;

                /* Copy values to intermediate in case of overlap */
                struct free_entry temp_entry = {
                    .size = p->size - size,
                };
                for (unsigned i = 0; i <= skip_list; i++) {
                    temp_entry.next[i] = p->next[i];
                }

                /* Set the values for the new entry */
                struct free_entry *new_entry = (void *)((uintptr_t)p + size);
                new_entry->size = temp_entry.size;
                /* Copy the next pointers that are valid for the new entry from p */
                const unsigned new_skip_list = which_skiplist_by_size(new_entry->size);
                for (unsigned i = 0; i <= new_skip_list; i++) {
                    new_entry->next[i] = temp_entry.next[i];
                }

                /* For pointers up to the new entry's skip list,
                 * point the previous block at the new entry
                 */
                for (unsigned i = 0; i <= new_skip_list; i++) {
                    *(links.lists[i]) = new_entry;
                }
                /* For pointers from the new entry's skip list up to p's
                 * skip list, point the previous block at what p pointed at
                 */
                for (unsigned i = (new_skip_list + 1); i <= skip_list; i++) {
                    *(links.lists[i]) = temp_entry.next[i];
                }
                break;
            }
        }
        advance_in_list(skip_list, (void *)&p, &links);
    }

    /* If we didn't find a valid spot, ret will be NULL here */
    return ret;
}

void *
ker_calloc(const size_t size) {
    void *p = ker_malloc(size);
    if (!p) {
        return p;
    }

    for (unsigned i = 0; i < size; i++) {
        char *c = p;
        c[i] = 0;
    }
    return p;
}

void *
ker_realloc(const size_t old_size, const size_t new_size, void *const p) {
}

void
ker_free(const size_t size, struct free_entry *const p) {
    const unsigned skip_list = which_skiplist_by_size(size);

    /* Go through lowest skip list to make sure we don't skip our spot */
    struct free_entry *traverse = free_list_start.next[0];
    struct list_links links;
    for (unsigned i = 0; i < NUM_FREE_LISTS; i++) {
        links.lists[i] = &free_list_start.next[i];
    }

    while (traverse) {
        if (p < traverse) {
            /* Freed memory block belongs just before traverse */

            /* Get pointer to block previous to p */
            const uintptr_t prev_int = (uintptr_t)(links.lists[0]);
            struct free_entry *prev = (void *)(prev_int - offsetof(struct free_entry, next));
            if ((((uintptr_t)prev) + size) == (uintptr_t)p) {
                /* Can coalesce freed block with previous block */
                prev->size += size;
                const unsigned prev_skip_list = which_skiplist_by_size(prev->size);

                if ((((uintptr_t)p) + size) == (uintptr_t)traverse) {
                    /* Can coalesce with next block */
                    prev->size += traverse->size;
                    const unsigned traverse_skip_list = which_skiplist_by_size(traverse->size);
                    const unsigned new_skip_list = which_skiplist_by_size(prev->size);

                    /* Update next pointers */
                    for (unsigned i = 0; i <= traverse_skip_list; i++) {
                        prev->next[i] = traverse->next[i];
                    }

                    for (unsigned i = (traverse_skip_list + 1); i <= new_skip_list; i++) {
                        prev->next[i] = *(links.lists[i]);
                    }

                    /* Set previous entries to point to prev */
                    for (unsigned i = (prev_skip_list + 1); i <= new_skip_list; i++) {
                        *(links.lists[i]) = prev;
                    }
                } else {
                    const unsigned new_skip_list = which_skiplist_by_size(prev->size);
                    for (unsigned i = (prev_skip_list + 1); i <= new_skip_list; i++) {
                        *(links.lists[i]) = prev;
                    }
                }
            } else {
                /* Can't coalesce, set values */
                p->size = size;

                if ((((uintptr_t)p) + size) == (uintptr_t)traverse) {
                    /* Can coalesce with next block */
                    p->size += traverse->size;
                    const unsigned traverse_skip_list = which_skiplist_by_size(traverse->size);
                    const unsigned new_skip_list = which_skiplist_by_size(p->size);

                    /* Update next pointers */
                    for (unsigned i = 0; i <= traverse_skip_list; i++) {
                        p->next[i] = traverse->next[i];
                    }

                    for (unsigned i = (traverse_skip_list + 1); i <= new_skip_list; i++) {
                        p->next[i] = *(links.lists[i]);
                    }

                    /* Set previous entries to point to p */
                    for (unsigned i = 0; i <= new_skip_list; i++) {
                        *(links.lists[i]) = p;
                    }
                } else {
                    /* Can't coalesce with any blocks, insert new one */
                    for (unsigned i = 0; i <= skip_list; i++) {
                        p->next[i] = *(links.lists[i]);
                        *(links.lists[i]) = p;
                    }
                }
            }
            break;
        }
        advance_in_list(0, (void *)&traverse, &links);
    }

    if (!traverse) {
        /* We got to the end of the list, so this block must belong on the end */

        /* Get pointer to block previous to p */
        const uintptr_t prev_int = (uintptr_t)(*(links.lists[0]));
        struct free_entry *prev = (void *)(prev_int - offsetof(struct free_entry, next));
        if ((((uintptr_t)prev) + size) == (uintptr_t)p) {
            /* Can coalesce with previous */
            prev->size += size;
        } else {
            /* Need to create new block */
            p->size = size;
            for (unsigned i = 0; i <= skip_list; i++) {
                p->next[i] = *(links.lists[i]);
                *(links.lists[i]) = p;
            }
        }
    }
}

/* Initializes structures required for allocator to work */
void
alloc_init(void) {
    ALLOCABLE_MEM_SIZE = ((uintptr_t)&_DATA_RAM_START) + SRAM_SIZE - ((uintptr_t)&_ALLOCABLE_MEM);

    struct free_entry *entry = ALLOCABLE_MEM_START;
    entry->size = ALLOCABLE_MEM_SIZE;

    for (unsigned i = 0; i < NUM_FREE_LISTS; i++) {
        free_list_start.next[i] = ALLOCABLE_MEM_START;
        entry->next[i] = NULL;
    }
}

