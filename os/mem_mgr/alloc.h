#ifndef ALLOC_H
#define ALLOC_H

#include <cstdio>

void *_ker_malloc(const size_t req_size);
void *_ker_calloc(const size_t req_size);
void _ker_free(const size_t req_size, void *const p);
void *_ker_realloc(const size_t old_size, const size_t new_size, void *const p);

void *_malloc(const size_t req_size);
void *_calloc(const size_t req_size);
void _free(void *const p);
void *_realloc(const size_t req_size, void *const p);
void alloc_init(void);

void *operator new(const size_t size);
void *operator new[](const size_t size);
void operator delete(void *const p) noexcept;
void operator delete[](void *const p) noexcept;
void operator delete(void *p, const size_t) noexcept;
void operator delete[](void *p, const size_t) noexcept;

#endif /* ALLOC_H */

