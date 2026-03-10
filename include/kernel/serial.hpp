#pragma once

#include <stdint.h>
#include <stddef.h>

namespace Kernel {
namespace Serial {
    constexpr uint16_t COM1_PORT = 0x3F8;
    
    void init();
    
    int is_transmit_empty();
    
    void write_char(char c);
    
    void write(const char* str);

    void write_hex(size_t hex);
} // namespace Serial
} // namespace Kernel