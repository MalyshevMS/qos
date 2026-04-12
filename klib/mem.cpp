#include <klib/mem.hpp>

void kstd::memcpy(void* dest, const void* source, size_t n) {
    for (int i = 0; i < n; i++)
    ((char*)dest)[i] = ((const char*)source)[i];
}

int kstd::memcmp(const void* s1, const void* s2, size_t n) {
    const unsigned char *p1 = (const unsigned char*)s1;
    const unsigned char *p2 = (const unsigned char*)s2;

    while (n--) {
        if (*p1 != *p2) {
            return (int)*p1 - (int)*p2;
        }
        p1++;
        p2++;
    }

    return 0;
}