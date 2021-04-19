#include <new>

#include "alloc.h"
#include "mem_mgr.h"
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

/* TODO: The way I expect locking to work:
 * 1) Skiplist locks itself on entry to malloc/realloc/free
 * 2) If it needs more memory, unlock and get memory from mem_mgr
 * 3) Call free on the new memory
 * 4) Call malloc again and return whatever is returned
 *
 * TODO: add allocator func to Skiplist
 */

/* TODO: refactors to this:
 * A lot of the helpers are gross and can be redone as combinations
 * of smaller functions. Changes I could make:
 *
 * allocate_entire_block: probably fine
 * allocate_current: probably fine
 * insert_new_block: maybe fine?
 *
 * insert_and_coalesce_with_current:
 *   - Shift curr_block backwards
 *   - Expand curr_block
 *
 * insert_and_coalesce_with_prev_and_current:
 *   - 2 options:
 *     1)
 *       - Expand prev
 *       - Merge prev with curr_block, where merge does:
 *         - Copies curr_block's next pointers
 *         - Expands prev
 *         - Updates links due to expansion
 *     2)
 *       - "Allocate" curr_block (have the next pointers skip over it)
 *       - Expand prev to cover freed block and curr_block
 *
 * expand_entry: probably fine
 * shrink_entry: probably fine
 *
 * copy_and_resize:
 *   - For expanding:
 *     - Shift passed in block backwards
 *     - Expand passed in block
 *   - For shrinking:
 *     - Shrink passed in block
 *     - Shift passed in block forwards
 *
 * resize_allocated_block:
 *   - For expanding:
 *     - Shrink following free block
 *     - Shift forward following free block
 *     - Return passed in pointer
 *   - For shrinking:
 *     - Shift backward following free block
 *     - Expand following free block
 *     - Return passed in pointer
 */

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

#define NUM_FREE_LISTS 4u
#define MIN_ALLOC_SIZE (sizeof(size_t) + sizeof(free_entry *))

#define MALLOC_HEADER_SIZE (2 * sizeof(size_t))
#define ALIGNMENT (sizeof(size_t))
#define ALIGNMENT_MASK (ALIGNMENT - 1u)
#define UNALIGNED(p) (((uintptr_t)p) & ALIGNMENT_MASK)

#define MIN_BLOCK_ALLOC_SIZE (2 * 1024)

static size_t
round_up_to_mult(const size_t value, const size_t mult_of)
{
    const size_t round_down = (value - 1u) & ~(mult_of - 1);
    return round_down + mult_of;
}

static unsigned
which_skiplist_by_size(const size_t size)
{
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

class Skiplist {
    public:
        Skiplist(void *(*const alloc_func)(const size_t size));
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
                void advance_links();

            public:
                list_walker(const unsigned skip_list, const Skiplist &list_start);
                bool fits_size(const size_t size) const { return curr_block->size >= size; };
                void move_next();
        };

        size_t total_mem;
        size_t total_free;
        free_entry *heads[NUM_FREE_LISTS];
        void *(*block_alloc_func)(const size_t size);

        list_walker get_walker(const unsigned skip_list) const;

        /* Helpers for malloc */
        void allocate_entire_block(list_walker& lw);
        void *allocate_current(list_walker& lw, const size_t size);

        /* Helpers for free */
        void insert_new_block(list_walker& lw, free_entry& new_block, const size_t size);
        void insert_and_coalesce_with_current(list_walker& lw, free_entry& entry, const size_t size);
        void insert_and_coalesce_with_prev_and_current(list_walker& lw, const size_t size, free_entry& prev);
        void expand_entry(list_walker& lw, free_entry& entry, const size_t expand_amt);

        /* Helpers for realloc */
        void copy_and_resize(list_walker& lw, free_entry& dest, const free_entry& src, const size_t new_size);
        void resize_allocated_block(list_walker& lw, const free_entry *const allocated_block, const size_t old_size, const size_t new_size);

        /* Unused - delete? */
        void shrink_entry(list_walker& lw, free_entry& entry, const size_t shrink_amt);
};

Skiplist::free_entry::free_entry(const free_entry& fe)
{
    copy_from(fe);
}

void
Skiplist::free_entry::copy_from(const free_entry& fe)
{
    size = fe.size;
    for (unsigned i = 0; i < fe.skiplist(); i++) {
        next[i] = fe.next[i];
    }
}

Skiplist::list_links::list_links(const Skiplist &list)
{
    for (unsigned i = 0; i < NUM_FREE_LISTS; i++) {
        lists[i] = const_cast<free_entry **>(&list.heads[i]);
    }
}

Skiplist::list_walker::list_walker(const unsigned skip_list, const Skiplist &list_start)
    : skiplist_num(skip_list),
      curr_block(list_start.heads[skip_list]),
      links(list_start) { }

void
Skiplist::list_walker::move_next()
{
    curr_block = curr_block->next[skiplist_num];
    advance_links();
}

void
Skiplist::list_walker::advance_links()
{
    for (unsigned i = 0; i < NUM_FREE_LISTS; i++) {
        if (*(links.lists[i]) < curr_block) {
            /* Advance the links forward, but only if they don't pass p.
             * This is because the links will be used to update the next
             * pointers in the list once an entry is allocated, so we need
             * to stay behind p.
             */
            struct free_entry *next_entry = *(links.lists[i]);
            links.lists[i] = &next_entry->next[i];
        }
    }
}

Skiplist::Skiplist(void *(*const alloc_func)(const size_t size))
    : total_mem(0),
      total_free(0),
      block_alloc_func(alloc_func)
{
    for (unsigned i = 0; i < NUM_FREE_LISTS; i++) {
        heads[i] = nullptr;
    }
}

Skiplist::list_walker
Skiplist::get_walker(const unsigned skip_list) const
{
    list_walker lw(skip_list, *this);
    return lw;
}

void
Skiplist::allocate_entire_block(list_walker& lw)
{
    for (unsigned i = 0; i <= lw.curr_block->skiplist(); i++) {
        *(lw.links.lists[i]) = lw.curr_block->next[i];
    }
}

void *
Skiplist::allocate_current(list_walker& lw, const size_t size)
{
    if (lw.curr_block->size < (size + MIN_ALLOC_SIZE)) {
        /*
         * If we allocated this block, the leftover would
         * be too small. Allocate the whole thing
         */

        /* Update pointers that pointed at p to point to the block after p */
        allocate_entire_block(lw);
    } else {
        /* We need to split the block */

        /* Copy values to intermediate in case of overlap */
        free_entry temp_entry(*(lw.curr_block));

        /* Set the values for the new entry */
        const uintptr_t curr_block_int = reinterpret_cast<uintptr_t>(lw.curr_block);
        free_entry *new_entry = reinterpret_cast<free_entry *>(curr_block_int + size);
        copy_and_resize(lw, *new_entry, temp_entry, temp_entry.size - size);
    }

    return lw.curr_block;
}

void
Skiplist::insert_new_block(list_walker& lw, free_entry& new_block, const size_t size)
{
    new_block.size = size;
    for (unsigned i = 0; i <= new_block.skiplist(); i++) {
        new_block.next[i] = *(lw.links.lists[i]);
        *(lw.links.lists[i]) = &new_block;
    }
}

void
Skiplist::insert_and_coalesce_with_current(list_walker& lw, free_entry& entry, const size_t size)
{
    entry.size = size + lw.curr_block->size;
    const unsigned curr_block_skiplist = lw.curr_block->skiplist();
    const unsigned new_skiplist = entry.skiplist();

    /* Update next pointers:
     *   - Need to copy the next pointers the curr_block had
     *   - For the other next pointers, copy the pointers from the links
     *   - Update links to point to entry up to its skiplist
     */
    for (unsigned i = 0; i <= curr_block_skiplist; i++) {
        entry.next[i] = lw.curr_block->next[i];
        *(lw.links.lists[i]) = &entry;
    }

    for (unsigned i = (curr_block_skiplist + 1); i <= new_skiplist; i++) {
        entry.next[i] = *(lw.links.lists[i]);
        *(lw.links.lists[i]) = &entry;
    }

    lw.curr_block = &entry;
}

void
Skiplist::insert_and_coalesce_with_prev_and_current(list_walker& lw, const size_t size, free_entry& prev)
{
    const unsigned prev_skiplist = prev.skiplist();
    prev.size += size + lw.curr_block->size;
    const unsigned curr_block_skiplist = lw.curr_block->skiplist();
    const unsigned new_skiplist = prev.skiplist();

    /* Update next pointers:
     *   - Copy all next pointers from curr_block (curr_block is at highest address)
     *   - For the rest, copy from the links
     *   - Update links to point to prev now that prev's skiplist has changed
     */
    for (unsigned i = 0; i <= curr_block_skiplist; i++) {
        prev.next[i] = lw.curr_block->next[i];
    }
    for (unsigned i = (curr_block_skiplist + 1); i <= new_skiplist; i++) {
        prev.next[i] = *(lw.links.lists[i]);
    }
    for (unsigned i = (prev_skiplist + 1); i <= new_skiplist; i++) {
        *(lw.links.lists[i]) = &prev;
    }

    lw.curr_block = &prev;
}

void
Skiplist::expand_entry(list_walker& lw, free_entry& entry, const size_t expand_amt)
{
    const unsigned old_skiplist = entry.skiplist();
    entry.size += expand_amt;
    const unsigned new_skiplist = entry.skiplist();

    for (unsigned i = (old_skiplist + 1); i <= new_skiplist; i++) {
        entry.next[i] = *(lw.links.lists[i]);
        *(lw.links.lists[i]) = &entry;
    }
}

void
Skiplist::shrink_entry(list_walker& lw, free_entry& entry, const size_t shrink_amt)
{
    const unsigned old_skiplist = entry.skiplist();
    entry.size -= shrink_amt;
    const unsigned new_skiplist = entry.skiplist();

    for (unsigned i = (new_skiplist + 1); i <= old_skiplist; i++) {
        *(lw.links.lists[i]) = entry.next[i];
    }
}

void
Skiplist::copy_and_resize(list_walker& lw, free_entry& dest, const free_entry& src, const size_t new_size)
{
    if (new_size == src.size) {
        dest.copy_from(src);
        return;
    }

    dest.size = new_size;
    const bool expanding = new_size > src.size;
    const unsigned old_skiplist = src.skiplist();
    const unsigned new_skiplist = dest.skiplist();

    if (expanding) {
        for (unsigned i = 0; i <= old_skiplist; i++) {
            dest.next[i] = src.next[i];
            *(lw.links.lists[i]) = &dest;
        }
        for (unsigned i = (old_skiplist + 1); i <= new_skiplist; i++) {
            dest.next[i] = *(lw.links.lists[i]);
            *(lw.links.lists[i]) = &dest;
        }
    } else {
        for (unsigned i = 0; i <= new_skiplist; i++) {
            dest.next[i] = src.next[i];
            *(lw.links.lists[i]) = &dest;
        }
        for (unsigned i = (new_skiplist + 1); i <= old_skiplist; i++) {
            *(lw.links.lists[i]) = src.next[i];
        }
    }
}

void
Skiplist::resize_allocated_block(list_walker& lw, const free_entry *const allocated_block, const size_t old_size, const size_t new_size)
{
    const uintptr_t ab_int = reinterpret_cast<uintptr_t>(allocated_block);
    const uintptr_t curr_block_int = reinterpret_cast<uintptr_t>(lw.curr_block);
    if ((ab_int + old_size) != curr_block_int) {
        /* allocated_block must be adjacent to the currently selected block */
        return;
    }

    if (old_size > new_size) {
        /* Shrinking p */
        const unsigned size_diff = old_size - new_size;
        /* Copy values over using temp as intermediary */
        free_entry temp(*(lw.curr_block));

        free_entry *new_block = reinterpret_cast<free_entry *>(curr_block_int - size_diff);
        copy_and_resize(lw, *new_block, temp, temp.size + size_diff);

        lw.curr_block = new_block;
    } else {
        /* Extending p */
        const unsigned size_diff = new_size - old_size;
        if ((lw.curr_block->size - size_diff) < MIN_ALLOC_SIZE) {
            /* Allocate all of curr_block */
            allocate_entire_block(lw);
        } else {
            /* Copy data into temp to avoid overlap */
            free_entry temp(*(lw.curr_block));

            free_entry *new_block = reinterpret_cast<free_entry *>(curr_block_int + size_diff);
            copy_and_resize(lw, *new_block, temp, temp.size - size_diff);

            lw.curr_block = new_block;
        }
    }
}

/*
 * The ker_* functions expect proper input values, should only be called
 * from the _* functions at the bottom of the file
 */
void *
Skiplist::malloc(const size_t size) {
    const unsigned skip_list = which_skiplist_by_size(size);

    list_walker lw = get_walker(skip_list);

    while (lw.curr_block) {
        if (lw.fits_size(size)) {
            /* We can use this block of memory */
            return allocate_current(lw, size);
        }
        lw.move_next();
    }

    /* Didn't find a valid spot */
    const size_t block_alloc_amt = round_up_to_mult(size, MIN_BLOCK_ALLOC_SIZE);
    void *const new_mem_block = block_alloc_func(block_alloc_amt);
    if (new_mem_block == nullptr) {
        /* Out of memory */
        return nullptr;
    }

    free(block_alloc_amt, new_mem_block);
    return malloc(size);
}

void
Skiplist::free(const size_t size, void *const pointer_to_free)
{
    free_entry *const p = static_cast<free_entry *>(pointer_to_free);

    /* Go through lowest skip list to make sure we don't skip our spot */
    list_walker lw = get_walker(0);

    while (lw.curr_block && (lw.curr_block <= p)) {
        lw.move_next();
    }

    /* Get pointer to block previous to p */
    const uintptr_t prev_int = reinterpret_cast<uintptr_t>(lw.links.lists[0]) - offsetof(free_entry, next);
    free_entry *const prev = reinterpret_cast<free_entry *>(prev_int);
    /* uint versions of pointers for comparisons */
    const uintptr_t p_int = reinterpret_cast<uintptr_t>(p);
    const uintptr_t curr_block_int = reinterpret_cast<uintptr_t>(lw.curr_block);

    if (lw.curr_block != nullptr) {
        /* Freed memory block belongs just before curr_block */

        if ((prev_int + prev->size) == p_int) {
            /* Can coalesce freed block with previous block */

            if ((p_int + size) == curr_block_int) {
                /* Can coalesce with next block */
                insert_and_coalesce_with_prev_and_current(lw, size, *prev);
            } else {
                /* Expand previous to fill the space */
                expand_entry(lw, *prev, size);
            }
        } else {
            /* Can't coalesce with previous, set values */
            if ((p_int + size) == curr_block_int) {
                /* Can coalesce with next block */
                insert_and_coalesce_with_current(lw, *p, size);
            } else {
                /* Can't coalesce with any blocks, insert new one */
                insert_new_block(lw, *p, size);
            }
        }
    } else {
        /* We got to the end of the list, so this block must belong on the end */
        if ((prev_int + size) == p_int) {
            /* Can coalesce with previous */
            expand_entry(lw, *prev, size);
        } else {
            /* Need to create new block */
            insert_new_block(lw, *p, size);
        }
    }
}

void *
Skiplist::resize(const size_t old_size, const size_t new_size, void *const pointer_to_resize)
{
    free_entry *const p = static_cast<free_entry *>(pointer_to_resize);
    const unsigned old_skip_list = which_skiplist_by_size(old_size);
    const bool expanding = new_size > old_size;

    list_walker lw = get_walker(old_skip_list);

    /* Find free block following p */
    while (lw.curr_block && (lw.curr_block <= p)) {
        lw.move_next();
    }

    /* Memory block belongs just before traverse */
    const uintptr_t p_int = reinterpret_cast<uintptr_t>(p);
    const uintptr_t curr_block_int = reinterpret_cast<uintptr_t>(lw.curr_block);
    if ((lw.curr_block != nullptr) && ((p_int + old_size) == curr_block_int)) {
        /* Following block is free and is adjacent to p, extend/shrink p */
        resize_allocated_block(lw, p, old_size, new_size);
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

            insert_new_block(lw, *new_block, size_diff);

            return p;
        }
    }

    return nullptr;
}

/* Entry point for each skip list */
static Skiplist free_list_start(nullptr);

/* Initializes structures required for allocator to work */
//TODO: skiplist needs an allocation function from mem_mgr to get blocks of mem
void
alloc_init(void) {
    free_list_start = Skiplist(allocatePages);
}

/* The _ker_* functions assume the caller enforces the restrictions
 * e.g. aligned sizes, aligned pointers
 */
void *
_ker_malloc(const size_t req_size)
{
    return free_list_start.malloc(req_size);
}

void *
_ker_calloc(const size_t req_size)
{
    size_t *p = static_cast<size_t *>(free_list_start.malloc(req_size));
    if (p == nullptr) {
        return nullptr;
    }

    /* p is assumed to be a multiple of size_t bytes */
    const size_t count = req_size / sizeof(size_t);
    for (unsigned i = 0; i < count; i++) {
        size_t *c = p;
        c[i] = 0;
    }
    return p;
}

void
_ker_free(const size_t req_size, void *const p)
{
    free_list_start.free(req_size, p);
}

void *
_ker_realloc(const size_t old_size, const size_t new_size, void *const p)
{
    size_t *ret = static_cast<size_t *>(free_list_start.resize(old_size, new_size, static_cast<void *>(p)));
    if (ret == nullptr) {
        /* Need to allocate new block */
        ret = static_cast<size_t *>(free_list_start.malloc(new_size));
        if (ret == nullptr) {
            /* Couldn't allocate more mem */
            return nullptr;
        }

        /* Copy data over */
        size_t copy_size;
        if (new_size < old_size)  {
            copy_size = new_size;
        } else {
            copy_size = old_size;
        }

        size_t count = copy_size / sizeof(size_t);
        size_t *const r = static_cast<size_t *>(ret);
        size_t *const c = static_cast<size_t *>(p);
        for (size_t i = 0; i < count; i++) {
            r[i] = c[i];
        }

        /* Free old mem */
        free_list_start.free(old_size, static_cast<void *>(p));

        return static_cast<void *>(r);
    }

    return ret;
}

void *
_malloc(const size_t req_size) {
    if (req_size == 0) {
        return NULL;
    }

    const size_t size = round_up_to_mult(req_size, ALIGNMENT) + MALLOC_HEADER_SIZE;

    size_t *p = static_cast<size_t *>(_ker_malloc(size));
    p[0] = size;
    const uintptr_t p_int = reinterpret_cast<uintptr_t>(p);
    return reinterpret_cast<void *>(p_int + MALLOC_HEADER_SIZE);
}

void *
_calloc(const size_t req_size) {
    if (req_size == 0) {
        return NULL;
    }

    const size_t size = round_up_to_mult(req_size, ALIGNMENT) + MALLOC_HEADER_SIZE;

    size_t *p = static_cast<size_t *>(_ker_calloc(size));
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

    _ker_free(size, static_cast<void *>(q));
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

    const size_t new_size = round_up_to_mult(req_size, ALIGNMENT) + MALLOC_HEADER_SIZE;
    if (new_size == old_size) {
        /* Same size requested, do nothing */
        return p;
    }
    /* Actual realloc */
    size_t *ret = static_cast<size_t *>(_ker_realloc(old_size, new_size, static_cast<void *>(q)));
    if (ret == nullptr) {
        return nullptr;
    }

    /* First slot is for storing size of block allocated */
    ret[0] = new_size;

    const uintptr_t ret_int = reinterpret_cast<uintptr_t>(ret);
    return reinterpret_cast<void *>(ret_int + MALLOC_HEADER_SIZE);
}

void *operator new(size_t size)
{
    void *p = _malloc(size);
    if (p == nullptr) {
        throw std::bad_alloc{};
    }
    return p;
}

void *operator new[](size_t size)
{
    void *p = _malloc(size);
    if (p == nullptr) {
        throw std::bad_alloc{};
    }
    return p;
}

void operator delete(void *p) noexcept
{
    _free(p);
}

void operator delete[](void *p) noexcept
{
    _free(p);
}

void operator delete(void *p, const size_t) noexcept
{
    _free(p);
}

void operator delete[](void *p, const size_t) noexcept
{
    _free(p);
}

