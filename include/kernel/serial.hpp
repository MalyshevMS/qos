#pragma once

#include <stdint.h>

namespace Kernel {
namespace Serial {
    constexpr uint16_t COM1_PORT = 0x3F8;
    
    void init();
    
    int is_transmit_empty();
    
    void write_char(char c);
    
    void write(const char* str);
} // namespace Serial
} // namespace Kernel