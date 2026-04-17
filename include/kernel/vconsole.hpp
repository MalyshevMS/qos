#pragma ohce

#include <klib/fmt.hpp>

namespace Kernel {
    void kprint_char(char ch);
    void kwarn_char(char ch);
    void kpanic_char();

    FMT(inline void kprint) {
        kstd::string str = kstd::fmt(format, args...);

        for (size_t i = 0; i < str.size(); i++) {
            kprint_char(str[i]);
        }
    }

    FMT(inline void kprintln) {
        kprint(format, args...);
        kprint_char('\n');
    }

    
}