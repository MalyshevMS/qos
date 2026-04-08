#include <klib/mem.hpp>

void kstd::memcpy(void* dest, const void* source, size_t n) {
    for (int i = 0; i < n; i++)
    ((char*)dest)[i] = ((const char*)source)[i];
}