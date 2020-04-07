#include "alloc.h"
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

#define MALLOC_HEADER_SIZE (2 * sizeof(size_t))
#define ALIGNMENT (sizeof(size_t))
#define ALIGNMENT_MASK (ALIGNMENT - 1u)
#define UNALIGNED(p) (((uintptr_t)p) & ALIGNMENT_MASK)
#define ROUND_UP_TO_ALIGN(size) ((((size) - 1u) & ~ALIGNMENT_MASK) + ALIGNMENT)

extern unsigned int _ALLOCABLE_MEM;
extern unsigned int _DATA_RAM_START;

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

struct free_entry;
struct list_links;
struct list_walker;
class Skiplist;

class Skiplist {
    public:
        void *malloc(const size_t size);
        void *resize(const size_t old_size, const size_t new_size, void *const p);
        void free(const size_t size, void *const p);

    private:
        /*
         * This struct will be placed at the beginning of each free entry.
         * Thus, it will never be allocated explicitly. A pointer to the
         * beginning of the entry will be cast to a pointer this header.
         *
         * Therefore, the size of the "next" array will depend on the size
         * of the entry itself. The array is declared as being of length
         * NUM_FREE_LISTS to supress compiler warnings.
         */
        struct free_entry {
            size_t size;
            free_entry *next[NUM_FREE_LISTS];

            public:
                free_entry(const free_entry& fe);
                void copy_from(const free_entry& fe);
                unsigned skiplist() const { return which_skiplist_by_size(size); };
        };

        struct list_links {
            /* Each pointer points to a next value of a free_entry element */
            free_entry **lists[NUM_FREE_LISTS];

            public:
                list_links(const Skiplist &list);
        };

        struct list_walker {
            unsigned skiplist_num;
            free_entry *curr_block;
            list_links links;

            private:
                void allocate_entire_block();

            public:
                list_walker(const unsigned skip_list, const Skiplist &list_start);
                bool fits_size(const size_t size) const { return curr_block->size >= size; };
                void move_next();
                void *allocate(const size_t size);

                void create_new_block(free_entry *const new_block, const size_t size);

                void resize_allocated_block(const free_entry *const allocated_block, const size_t old_size, const size_t new_size);
                void free(const size_t size);
        };

        size_t total_mem;
        size_t total_free;
        free_entry *heads[NUM_FREE_LISTS];

        void expand_entry(free_entry *const p, const size_t expand_amt, list_walker &iter);
        list_walker get_walker(const int skip_list) const;

        /* TODO: clean these function up */
        void free_entry::copy_and_resize(const free_entry *const fe, const size_t new_size, list_links &links);
        void free_entry::resize(const size_t new_size, list_links &links);
        void list_links::advance_in_list(const free_entry *const dont_pass);
};

free_entry::free_entry(const free_entry *const fe)
{
    copy_from(fe);
}

void
free_entry::copy_from(const free_entry *const fe)
{
    size = fe->size;
    for (unsigned i = 0; i < fe->skiplist(); i++) {
        next[i] = fe->next[i];
    }
}

/* links must point to the next pointers previous to this */
void
free_entry::copy_and_resize(const free_entry *const fe, const size_t new_size, list_links &links)
{
    if (new_size == fe->size) {
        copy_from(fe);
        return;
    }

    size = new_size;
    const bool expanding = new_size > fe->size;
    const unsigned old_skip_list = fe->skiplist();
    const unsigned new_skip_list = skiplist();

    if (expanding) {
        for (unsigned i = 0; i <= old_skip_list; i++) {
            next[i] = fe->next[i];
            *(links.lists[i]) = this;
        }
        for (unsigned i = (old_skip_list + 1); i <= new_skip_list; i++) {
            next[i] = *(links.lists[i]);
            *(links.lists[i]) = this;
        }
    } else {
        for (unsigned i = 0; i <= new_skip_list; i++) {
            next[i] = fe->next[i];
            *(links.lists[i]) = this;
        }
        for (unsigned i = (new_skip_list + 1); i <= old_skip_list; i++) {
            *(links.lists[i]) = fe->next[i];
        }
    }
}

/* links must point to the next pointers previous to this */
void
free_entry::resize(const size_t new_size, list_links &links)
{
    if (new_size == size) {
        return;
    }
    const bool expanding = new_size > size;

    const unsigned old_skip_list = skiplist();
    size = new_size;
    const unsigned new_skip_list = skiplist();

    if (expanding) {
        for (unsigned i = (old_skip_list + 1); i <= new_skip_list; i++) {
            next[i] = *(links.lists[i]);
            *(links.lists[i]) = this;
        }
    } else {
        for (unsigned i = (new_skip_list + 1); i <= old_skip_list; i++) {
            *(links.lists[i]) = this;
        }
    }
}

list_links::list_links(const Skiplist &list)
{
    for (unsigned i = 0; i < NUM_FREE_LISTS; i++) {
        lists[i] = const_cast<free_entry **>(&list.heads[i]);
    }
}

list_links::list_links(const free_entry &fe)
{
    for (unsigned i = 0; i < NUM_FREE_LISTS; i++) {
        lists[i] = const_cast<free_entry **>(&fe.next[i]);
    }
}

void
list_links::advance_in_list(const free_entry *const dont_pass)
{
    for (unsigned i = 0; i < NUM_FREE_LISTS; i++) {
        if (*(lists[i]) < dont_pass) {
            /* Advance the links forward, but only if they don't pass p.
             * This is because the links will be used to update the next
             * pointers in the list once an entry is allocated, so we need
             * to stay behind p.
             */
            struct free_entry *next_entry = *(lists[i]);
            lists[i] = &next_entry->next[i];
        }
    }
}

list_walker::list_walker(const unsigned skip_list, const Skiplist &list_start)
    : skiplist_num(skip_list),
      curr_block(list_start.heads[skip_list]),
      links(list_start)
{
}

void
list_walker::create_new_block(free_entry *const new_block, const size_t size)
{
    new_block->size = size;
    for (unsigned i = 0; i <= new_block->skiplist(); i++) {
        new_block->next[i] = *(links.lists[i]);
        *(links.lists[i]) = new_block;
    }
}

void
list_walker::allocate_entire_block()
{
    for (unsigned i = 0; i <= curr_block->skiplist(); i++) {
        *(links.lists[i]) = curr_block->next[i];
    }
}

void
list_walker::move_next()
{
    curr_block = curr_block->next[skiplist_num];
    links.advance_in_list(curr_block);
}

void *
list_walker::allocate(const size_t size)
{
    if (curr_block->size < (size + MIN_ALLOC_SIZE)) {
        /*
         * If we allocated this block, the leftover would
         * be too small. Allocate the whole thing
         */

        /* Update pointers that pointed at p to point to the block after p */
        allocate_entire_block();
    } else {
        /* We need to split the block */

        /* Copy values to intermediate in case of overlap */
        free_entry temp_entry(curr_block);

        /* Set the values for the new entry */
        const uintptr_t curr_block_int = reinterpret_cast<uintptr_t>(curr_block);
        free_entry *new_entry = reinterpret_cast<free_entry *>(curr_block_int + size);
        new_entry->copy_and_resize(&temp_entry, temp_entry.size - size, links);
    }

    return curr_block;
}

void
list_walker::resize_allocated_block(const free_entry *const allocated_block, const size_t old_size, const size_t new_size)
{
    const uintptr_t ab_int = reinterpret_cast<uintptr_t>(allocated_block);
    const uintptr_t curr_block_int = reinterpret_cast<uintptr_t>(curr_block);
    if ((ab_int + old_size) != curr_block_int) {
        /* allocated_block must be adjacent to the currently selected block */
        return;
    }

    if (old_size > new_size) {
        /* Shrinking p */
        const unsigned size_diff = old_size - new_size;
        /* Copy values over using temp as intermediary */
        free_entry temp(curr_block);

        free_entry *new_block = reinterpret_cast<free_entry *>(curr_block_int - size_diff);
        new_block->copy_and_resize(&temp, temp.size - size_diff, links);

        curr_block = new_block;
    } else {
        /* Extending p */
        const unsigned size_diff = new_size - old_size;
        if ((curr_block->size - size_diff) < MIN_ALLOC_SIZE) {
            /* Allocate all of curr_block */
            allocate_entire_block();
        } else {
            /* Copy data into temp to avoid overlap */
            free_entry temp(curr_block);

            free_entry *new_block = reinterpret_cast<free_entry *>(curr_block_int + size_diff);
            new_block->copy_and_resize(&temp, temp.size - size_diff, links);

            curr_block = new_block;
        }
    }
}

void
list_walker::free(const size_t size)
{
}

list_walker
Skiplist::get_walker(const int skip_list)
{
    list_walker iter(skip_list, *this);
    return iter;
}

/* Start of allocable memory block (and size), maybe make this a macro? */
static void *const ALLOCABLE_MEM_START = &_ALLOCABLE_MEM;
static size_t ALLOCABLE_MEM_SIZE;

/* Entry point for each skip list */
static Skiplist free_list_start;

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
 *      - Allocates size + MALLOC_HEADER_SIZE, stores the allocated size in the 4 B
 *        preceding the returned pointer
 *  - May need similar thing for realloc - inner function accepts old memory size
 *    as well as new memory size
 *  - Start location of allocable memory will be communicated by the linker script
 *    (by communicating end of memory used in the OS binary) and size will be
 *    computed from this
 * TODO: make sure allocated blocks are multiples of 4 B for alignment and also
 * that freed pointers are aligned correctly (and realloc'd ones too)
 */

/*
 * The ker_* functions expect proper input values, should only be called
 * from the _* functions at the bottom of the file
 */
void *
Skiplist::malloc(const size_t size) {
    const unsigned skip_list = which_skiplist_by_size(size);

    list_walker iter = get_walker(skip_list);

    while (iter.curr_block) {
        if (iter.fits_size(size)) {
            /* We can use this block of memory */
            return iter.allocate(size);
        }
        iter.move_next();
    }

    /* Didn't find a valid spot */
    return nullptr;
}

void
Skiplist::free(const size_t size, free_entry *const p)
{
    const unsigned skip_list = which_skiplist_by_size(size);

    /* Go through lowest skip list to make sure we don't skip our spot */
    list_walker iter = get_walker(0);

    while (iter.curr_block && (iter.curr_block <= p)) {
        iter.move_next();
    }

    const uintptr_t p_int = reinterpret_cast<uintptr_t>(p);
    const uintptr_t curr_block_int = reinterpret_cast<uintptr_t>(iter.curr_block);
    if (iter.curr_block) {
        /* Freed memory block belongs just before traverse */

        /* Get pointer to block previous to p */
        const uintptr_t prev_int = reinterpret_cast<uintptr_t>(links.lists[0]) - offsetof(free_entry, next);
        free_entry *prev = reinterpret_cast<free_entry *>(prev_int);
        if ((prev + prev->size) == p_int) {
            /* Can coalesce freed block with previous block */
            const unsigned prev_skip_list = prev->skiplist();
            prev->size += size;

            if ((p_int + size) == curr_block_int) {
                /* Can coalesce with next block */
                prev->size += iter.curr_block->size;
                const unsigned curr_block_skip_list = iter.curr_block->skiplist();
                const unsigned new_skip_list = prev->skiplist();

                /* Update next pointers */
                for (unsigned i = 0; i <= curr_block_skip_list; i++) {
                    prev->next[i] = iter.curr_block->next[i];
                }

                for (unsigned i = (curr_block_skip_list + 1); i <= new_skip_list; i++) {
                    prev->next[i] = *(links.lists[i]);
                }

                /* Set previous entries to point to prev */
                for (unsigned i = (prev_skip_list + 1); i <= new_skip_list; i++) {
                    *(links.lists[i]) = prev;
                }
            } else {
                const unsigned new_skip_list = prev->skiplist();
                for (unsigned i = (prev_skip_list + 1); i <= new_skip_list; i++) {
                    prev->next[i] = *(links.lists[i]);
                    *(links.lists[i]) = prev;
                }
            }
        } else {
            /* Can't coalesce, set values */
            p->size = size;

            if ((p_int + size) == (uintptr_t)iter.curr_block) {
                /* Can coalesce with next block */
                p->size += traverse->size;
                const unsigned traverse_skip_list = which_skiplist_by_size(iter.curr_block->size);
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
                iter.create_new_block(p, size);
            }
        }
    } else {
        /* We got to the end of the list, so this block must belong on the end */

        /* Get pointer to block previous to p */
        const uintptr_t prev_int = (uintptr_t)(*(links.lists[0]));
        struct free_entry *prev = (void *)(prev_int - offsetof(struct free_entry, next));
        if ((((uintptr_t)prev) + size) == p_int) {
            /* Can coalesce with previous */
            prev->size += size;
        } else {
            /* Need to create new block */
            iter.create_new_block(p, size);
        }
    }
}

void *
Skiplist::resize(const size_t old_size, const size_t new_size, free_entry *const p)
{
    //TODO: this can be simplified a lot.
    // if (expanding) {
    //     new_p = malloc(new_size);
    //     copy_data();
    //     return new_p;
    // } else {
    //     if (size_diff < MIN_ALLOC_SIZE) return p;
    //     else {
    //         free(p + size_diff, size_diff);
    //         p->size -= size_diff;
    //     }
    // }
    const unsigned old_skip_list = which_skiplist_by_size(old_size);
    const bool expanding = new_size > old_size;

    list_walker iter = get_walker(old_skip_list);

    /* Find free block following p */
    while (iter.curr_block && (iter.curr_block <= p)) {
        iter.move_next();
    }

    /* Memory block belongs just before traverse */
    const uintptr_t p_int = reinterpret_cast<uintptr_t>(p);
    const uintptr_t curr_block_int = reinterpret_cast<uintptr_t>(iter.curr_block);
    if ((iter.curr_block != nullptr) && ((p_int + old_size) == curr_block_int)) {
        /* Following block is free and is adjacent to p, extend/shrink p */
        iter.resize_allocated_block(p, old_size, new_size);
        return p;
    } else {
        /* Not connected, will need to create a new free_entry */
        if (expanding) {
            /* Can't resize, caller will need to allocate new block,
             * copy data over, then free the old one.
             */
            return nullptr;
        } else {
            /* If the size difference is < min allocable size, just return p */
            const size_t size_diff = old_size - new_size;
            if (size_diff < MIN_ALLOC_SIZE) {
                return p;
            }
            /* Create a new block following p */
            const uintptr_t new_block_int = p_int + new_size;
            free_entry *new_block = reinterpret_cast<free_entry *>(new_block_int);

            iter.create_new_block(new_block, size_diff);

            return p;
        }
    }

    return nullptr;
}

/* Initializes structures required for allocator to work */
//TODO: this list will be used for heap allocations and will need memory to be allocated from mem_mgr before use
//TODO: move ALLOCABLE_MEM_SIZE to mem_mgr
void
alloc_init(void) {
    ALLOCABLE_MEM_SIZE = ((uintptr_t)&_DATA_RAM_START) + SRAM_SIZE - ((uintptr_t)&_ALLOCABLE_MEM);

    free_entry *entry = reinterpret_cast<free_entry *>(ALLOCABLE_MEM_START);
    entry->size = ALLOCABLE_MEM_SIZE;

    for (unsigned i = 0; i < NUM_FREE_LISTS; i++) {
        free_list_start.heads[i] = entry;
        entry->next[i] = nullptr;
    }
}

void *
_malloc(const size_t req_size) {
    if (req_size == 0) {
        return NULL;
    }

    const size_t size = ROUND_UP_TO_ALIGN(req_size) + MALLOC_HEADER_SIZE;

    size_t *p = static_cast<size_t *>(free_list_start.malloc(size));
    p[0] = size;
    const uintptr_t p_int = reinterpret_cast<uintptr_t>(p);
    return reinterpret_cast<void *>(p_int + MALLOC_HEADER_SIZE);
    /* The way I expect locking to work:
     * 1) lock
     * 2) ker_malloc()
     * 3) unlock
     * 4) If p == NULL:
     *     a) allocate more memory to heap
     *     b) lock
     *     c) ker_free() on newly allocated memory
     *     d) unlock
     *     e) lock
     *     f) ker_malloc()
     *     g) unlock
     *     h) if p == NULL return NULL else return p
     * 5) Else return p
     */
}

void *
_calloc(const size_t req_size) {
    if (req_size == 0) {
        return NULL;
    }

    const size_t size = ROUND_UP_TO_ALIGN(req_size) + MALLOC_HEADER_SIZE;

    size_t *p = static_cast<size_t *>(free_list_start.malloc(size));
    if (p == nullptr) {
        return nullptr;
    }

    /* p is guaranteed to be a multiple of size_t bytes */
    const size_t count = size / ALIGNMENT;
    for (unsigned i = 1; i < count; i++) {
        size_t *c = p;
        c[i] = 0;
    }
    p[0] = size;
    const uintptr_t p_int = reinterpret_cast<uintptr_t>(p);
    return reinterpret_cast<void *>(p_int + MALLOC_HEADER_SIZE);
}

void
_free(void *const p) {
    if (UNALIGNED(p) || (p == nullptr)) {
        /* Just do nothing... not sure what the "right" thing is */
        return;
    }
    const uintptr_t p_int = reinterpret_cast<uintptr_t>(p);
    size_t *const q = reinterpret_cast<size_t *>(p_int - MALLOC_HEADER_SIZE);
    const size_t size = q[0];

    free_list_start.free(size, reinterpret_cast<free_entry *>(q));
}

void *
_realloc(const size_t req_size, void *const p) {
    if (!p) {
        return _malloc(req_size);
    } else if (req_size == 0) {
        /* p is a valid pointer and requested size is zero: free block of memory */
        _free(p);
        return nullptr;
    } else if (UNALIGNED(p)) {
        /* Just do nothing... not sure what the "right" thing is */
        return p;
    }

    const uintptr_t p_int = reinterpret_cast<uintptr_t>(p);
    size_t *const q = reinterpret_cast<size_t *>(p_int - MALLOC_HEADER_SIZE);
    const size_t old_size = q[0];
    const size_t new_size = ROUND_UP_TO_ALIGN(req_size) + MALLOC_HEADER_SIZE;
    if (new_size == old_size) {
        /* Same size requested, do nothing */
        return p;
    }
    /* Actual realloc */
    size_t *ret = static_cast<size_t *>(free_list_start.resize(old_size, new_size, reinterpret_cast<free_entry *>(q)));
    if (ret == nullptr) {
        /* Need to allocate new block */
        ret = static_cast<size_t *>(free_list_start.malloc(new_size));
        if (ret == nullptr) {
            /* Couldn't allocate more mem */
            return nullptr;
        }
        /* First slot is for storing size of block allocated */
        ret[0] = new_size;

        /* Copy data over */
        size_t copy_size;
        if (new_size < old_size)  {
            copy_size = new_size;
        } else {
            copy_size = old_size;
        }

        size_t count = (copy_size - MALLOC_HEADER_SIZE) / sizeof(size_t);
        const uintptr_t ret_int = reinterpret_cast<uintptr_t>(ret);
        size_t *const r = reinterpret_cast<size_t *>(ret_int + MALLOC_HEADER_SIZE);
        size_t *const c = static_cast<size_t *>(p);
        for (size_t i = 0; i < count; i++) {
            r[i] = c[i];
        }

        /* Free old mem */
        free_list_start.free(old_size, reinterpret_cast<free_entry *>(q));

        return static_cast<void *>(r);
    }

    return ret;
}

