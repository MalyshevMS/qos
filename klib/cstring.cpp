#include <klib/cstring.hpp>

size_t kstd::strlen(const char* str) {
    size_t size = 0;
    while (str[++size] != 0);
    return size;
}

void kstd::itoa(int value, char *str, int base) {
    char* ptr = str;
    char* start = str;
    char negative = 0;

    // Handle negative numbers (only meaningful for base 10)
    if (value < 0 && base == 10) {
        negative = 1;
        value = -value; // Note: INT_MIN will overflow in a simple impl
    }

    // Extract digits in reverse order
    do {
        int rem = value % base;
        *ptr++ = (rem < 10) ? (rem + '0') : (rem - 10 + 'a');
        value /= base;
    } while (value > 0);

    if (negative) *ptr++ = '-';
    *ptr = 0;

    // Reverse the string
    char* end = ptr - 1;
    while (start < end) {
        char tmp = *start;
        *start = *end;
        *end = tmp;
        start++;
        end--;
    }
}
