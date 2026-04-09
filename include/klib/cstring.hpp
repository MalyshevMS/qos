#pragma once

#include <stddef.h>

namespace kstd {
    size_t strlen(const char* str);

    void itoa(int value, char* str, int base);
} // namespace kstd