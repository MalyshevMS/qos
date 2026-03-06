#pragma once

#include <stdint.h>

namespace Serial {
    constexpr uint16_t COM1_PORT = 0x3F8;

    static void outb(uint16_t port, uint8_t val);
    
    static uint8_t inb(uint16_t port);
    
    void init();
    
    int is_transmit_empty();
    
    void write_char(char c);
    
    void write(const char* str);
}
