#pragma once

#include <cstddef>

namespace kstd {
    size_t strlen(const char* str);

    void itoa(int value, char* str, int base);
    void utoa(unsigned int value, char* str, int base);
    void ltoa(long value, char* str, int base);
    void ultoa(unsigned long value, char* str, int base);
    void lltoa(long long value, char* str, int base);
    void ulltoa(unsigned long long value, char* str, int base);
} // namespace kstd