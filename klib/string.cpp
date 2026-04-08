#include <klib/string.hpp>
#include <klib/cstring.hpp>
#include <klib/mem.hpp>

using namespace kstd;

string::string(const char* cstr) {
    len = strlen(cstr);
    head = new char[len];
    memcpy(head, cstr, len);
}

char string::operator[](int index) {
    if (index > len) return 0;
    return *(head + index);
}

const char* string::c_str() {
    auto res = new char[len + 1];
    memcpy(res, head, len);
    res[len] = 0; // Null terminator
    return res;
}