#include <klib/cstring.hpp>

size_t kstd::strlen(const char* str) {
    if (!str) return 0;
    size_t size = 0;
    while (str[size] != '\0') size++;
    return size;
}

void kstd::itoa(int value, char *str, int base) {
    if (!str) return;
    
    char* ptr = str;
    char* start = str;
    int negative = 0;

    if (value < 0 && base == 10) {
        negative = 1;
        value = -value;
    }

    if (value == 0) {
        *ptr++ = '0';
    } else {
        while (value > 0) {
            int rem = value % base;
            *ptr++ = (rem < 10) ? (rem + '0') : (rem - 10 + 'a');
            value /= base;
        }
    }

    if (negative) *ptr++ = '-';
    *ptr = '\0';

    ptr--;
    while (start < ptr) {
        char tmp = *start;
        *start = *ptr;
        *ptr = tmp;
        start++;
        ptr--;
    }
}
