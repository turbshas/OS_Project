#ifndef ALLOC_H
#define ALLOC_H

#include "mem_region.hpp"
#include <cstdio>

using AllocFunc = MemRegion (*const)(const size_t size);
using AllocCompleteCallback = void (*const)(const MemRegion& memRegion);

void* _ker_malloc(const size_t req_size);
void* _ker_calloc(const size_t req_size);
void _ker_free(const size_t req_size, void* const p);
void* _ker_realloc(const size_t old_size, const size_t new_size, void* const p);

void* _malloc(const size_t req_size);
void* _calloc(const size_t req_size);
void _free(void* const p);
void* _realloc(const size_t req_size, void* const p);
void alloc_init(AllocFunc alloc_func, AllocCompleteCallback callback);

#endif /* ALLOC_H */
