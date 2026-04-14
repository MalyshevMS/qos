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

void kstd::utoa(unsigned int value, char* str, int base) {
    if (!str) return;
    if (base < 2) base = 2;
    if (base > 36) base = 36;

    const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    int i = 0;

    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    do {
        str[i++] = digits[value % (unsigned int)base];
        value /= (unsigned int)base;
    } while (value > 0);

    str[i] = '\0';

    int start = 0;
    int end = i - 1;
    while (start < end) {
        char tmp = str[start];
        str[start] = str[end];
        str[end] = tmp;
        start++;
        end--;
    }
}

void kstd::ltoa(long value, char* str, int base) {
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
            long rem = value % base;
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

void kstd::ultoa(unsigned long value, char* str, int base) {
    if (!str) return;
    if (base < 2) base = 2;
    if (base > 36) base = 36;

    const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    int i = 0;

    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    do {
        str[i++] = digits[value % (unsigned long)base];
        value /= (unsigned long)base;
    } while (value > 0);

    str[i] = '\0';

    int start = 0;
    int end = i - 1;
    while (start < end) {
        char tmp = str[start];
        str[start] = str[end];
        str[end] = tmp;
        start++;
        end--;
    }
}



void kstd::lltoa(long long value, char* str, int base) {
    // Not supported in kernel - use lesser types instead
    if (!str) return;
    itoa((int)value, str, base);
}

void kstd::ulltoa(unsigned long long value, char* str, int base) {
    // Not supported in kernel - use lesser types instead
    if (!str) return;
    utoa((unsigned int)value, str, base);
}