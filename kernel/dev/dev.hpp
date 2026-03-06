#pragma once
#include <stdint.h>

namespace Dev {
    static void outb(uint16_t port, uint8_t val);
    
    static uint8_t inb(uint16_t port);
}