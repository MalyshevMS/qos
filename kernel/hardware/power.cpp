#include <kernel/power.hpp>
#include <kernel/ports.hpp>
#include <cstdint>

using namespace Kernel;
using namespace Ports;

void Hardware::reboot() {
    uint8_t good = 0x02;
    while (good & 0x02) good = inb(0x64);
    outb(0x64, 0xFE);
}