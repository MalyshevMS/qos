#pragma once
#include <stddef.h>

namespace Mem {
    void init();
    void* malloc(size_t size);
    void free(void* ptr);
}