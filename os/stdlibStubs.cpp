#include <cstdlib>
#include <new>

#include "alloc.h"

void *operator new(size_t size) noexcept
{
    void *p = _ker_malloc(size);
    return p;
}

void* operator new(size_t size, std::nothrow_t) noexcept
{
    return operator new(size); // Same as regular new
}

void *operator new[](size_t size) noexcept
{
    return operator new(size); // Same as regular new
}

void* operator new[](size_t size, std::nothrow_t) noexcept
{
    return operator new(size); // Same as regular new
}

void operator delete(void *) noexcept
{
    // Required if a class has a virtual destructor.
    // Can't really do anything here as no size is known and only _ker_malloc should be used
    // within the OS for allocating mem as it saves space by avoiding the required size storage.
}

void operator delete(void *p,  std::nothrow_t) noexcept
{
    operator delete(p); // Same as regular delete
}

void operator delete(void *p, const size_t req_size) noexcept
{
    _ker_free(req_size, p);
}

void operator delete(void *p, const size_t req_size, std::nothrow_t) noexcept
{
    operator delete(p, req_size); // Same as regular delete
}

void operator delete[](void *p) noexcept
{
    operator delete(p); // Same as regular delete
}

void operator delete[](void *p,  std::nothrow_t) noexcept
{
    operator delete(p); // Same as regular delete
}

void operator delete[](void *p, const size_t req_size) noexcept
{
    operator delete(p, req_size); // Same as regular delete
}

void operator delete[](void *p, const size_t req_size, std::nothrow_t) noexcept
{
    operator delete(p, req_size); // Same as regular delete
}

extern "C" int __aeabi_atexit( 
    void *object, 
    void (*destructor)(void *), 
    void *dso_handle) 
{ 
    static_cast<void>(object); 
    static_cast<void>(destructor); 
    static_cast<void>(dso_handle); 
    return 0; 
} 

void* __dso_handle = nullptr;

extern "C" void __cxa_pure_virtual()
{
    while (true) {}
}
