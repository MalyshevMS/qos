#pragma once
#include <cstddef>
#include <cstdint>

namespace Kernel {
namespace Mem {
    void meminit();
    void* malloc(size_t size);
    void free(void* ptr);
    uint64_t leaked();
} // namespace Mem
} // namespace Kernel

void* operator new(size_t size);
void operator delete(void* ptr);
void operator delete(void* ptr, size_t size);

void* operator new[](size_t size);
void operator delete[](void* ptr);