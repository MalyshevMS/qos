#include <kernel/memory.hpp>

using namespace Kernel::Mem;

void *operator new(size_t size) {
    return malloc(size);
}

void operator delete(void *ptr) {
    if (ptr) free(ptr);
}

void *operator new[](size_t size) {
    return malloc(size);
}

void operator delete[](void *ptr) {
    if (ptr) free(ptr);
}

void operator delete(void *ptr, size_t size) {
    if (ptr) free(ptr);
}