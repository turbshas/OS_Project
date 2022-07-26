#include "mem_mgr.h"
#include "pageList.h"

PageList::PageSequence::PageSequence()
    : numPages(0),
    prev(this),
    next(this)
{
}

PageList::PageSequence::~PageSequence()
{
    // Pages are cast from pointers to unused memory, not constructed.
    // So nothing to deconstruct.
}

void
PageList::PageSequence::insertAfter(PageSequence& insert)
{
    insert.next = next;
    insert.prev = this;
    insert.next->prev = &insert;
    next = &insert;
}

void
PageList::PageSequence::insertBefore(PageSequence& insert)
{
    insert.next = this;
    insert.prev = prev;
    insert.prev->next = &insert;
    prev = &insert;
}

PageList::PageSequence *
PageList::PageSequence::remove()
{
    prev->next = next;
    next->prev = prev;
    next = nullptr;
    prev = nullptr;
    return this;
}

PageList::PageList()
    : sentinel()
{
}

PageList::~PageList()
{
    // Page list is essentially composed of pointers to unused memory, so nothing is allocated.
    // Therefore, nothing to deconstruct.
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
