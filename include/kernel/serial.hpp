#pragma once

#include <klib/fmt.hpp>
#include <kernel/vconsole.hpp>
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
    
    void print(const kstd::string& text);
    void println(const kstd::string& text = "");
} // namespace Serial
} // namespace Kernel