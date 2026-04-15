#pragma once

#include <klib/fmt.hpp>
#include <cstdint>
#include <cstddef>


namespace Kernel {
namespace Serial {
    constexpr uint16_t COM1_PORT = 0x3F8;
    
    void init();
    
    int is_transmit_empty();
    
    void write_char(char c);
    
    void write(const char* str);

    void write_hex(size_t hex);

    FMT(inline void print) { // Forced to write here because of templates
        Serial::write(kstd::fmt(format, args...).c_str());
    }

    FMT(inline void println) {
        Serial::write(kstd::fmt(format, args...).c_str());
        Serial::write_char('\n');
    }

    inline void println() {
        Serial::write_char('\n');
    }
} // namespace Serial
} // namespace Kernel