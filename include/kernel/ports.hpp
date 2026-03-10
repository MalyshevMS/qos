#pragma once
#include <stdint.h>

namespace Kernel {
namespace Ports {
    void outb(uint16_t port, uint8_t val);
    
    uint8_t inb(uint16_t port);
} // namespace Ports
} // namespace Kernel