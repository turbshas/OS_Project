#ifndef _PAGE_LIST_H
#define _PAGE_LIST_H

#include <cstdio>

class PageList {
    // Will be casting the start of each sequence to a pointer to one of these
    struct PageSequence {
        uint32_t numPages;
        PageSequence *prev;
        PageSequence *next;

        PageSequence();
        PageSequence(const PageSequence&) = delete;
        PageSequence(PageSequence&&) = delete;
        ~PageSequence();
        PageSequence& operator=(const PageSequence&) = delete;
        PageSequence& operator=(PageSequence&&) = delete;

        void insertAfter(PageSequence& insert);
        void insertBefore(PageSequence& insert);
        PageSequence *remove();
    };

    PageSequence sentinel;

    bool areSequencesAdjacent(const PageSequence& first, const PageSequence& second) const;

    public:
        PageList();
        ~PageList();
        void initialize(const size_t numPages, void *const startAddr);
        void *allocatePages(const size_t numPages);
        void freePages(const size_t numPages, void *startAddr);
};

#endif
