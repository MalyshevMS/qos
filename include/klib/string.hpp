#pragma once

#include <stddef.h>

namespace kstd {
class string {
private:
    char* head;
    size_t len;
public:
    string(const char* cstr);
    
    char operator[](int index);

    const char* c_str();

    size_t size() { return len; }
};
} // namespace std