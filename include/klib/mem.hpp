#pragma once

#include <cstddef>

namespace kstd {
    void memcpy(void* dest, const void* source, size_t n);
    int memcmp(const void* s1, const void* s2, size_t n);
} // namespace kstd