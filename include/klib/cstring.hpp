#pragma once

#include <cstddef>

namespace kstd {
    size_t strlen(const char* str);

    void itoa(int value, char* str, int base = 10);
    void utoa(unsigned int value, char* str, int base = 10);
    void ltoa(long value, char* str, int base = 10);
    void ultoa(unsigned long value, char* str, int base = 10);
    void lltoa(long long value, char* str, int base = 10);
    void ulltoa(unsigned long long value, char* str, int base = 10);
} // namespace kstd