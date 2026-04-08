#include <klib/cstring.hpp>

size_t kstd::strlen(const char* str) {
    size_t size = 0;
    while (str[++size] != 0);
    return size;
}