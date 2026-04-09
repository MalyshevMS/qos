#pragma once
#include <cstddef>

namespace Kernel {
namespace Mem {
    void meminit();
    void* malloc(size_t size);
    void free(void* ptr);

} // namespace Mem
} // namespace Kernel

void* operator new(size_t size);
void operator delete(void* ptr);

void* operator new[](size_t size);
void operator delete[](void* ptr);