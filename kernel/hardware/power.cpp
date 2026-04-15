#include <kernel/power.hpp>
#include <kernel/ports.hpp>
#include <kernel/serial.hpp>
#include <driver/acpi.hpp>
#include <cfg/asm.txx>
#include <cstdint>

using namespace Kernel;
using namespace Ports;
using namespace Driver;

namespace Kernel::Hardware {

void poweroff() {
    // Try ACPI first, don't disable interrupts yet
    if (ACPI::poweroff()) {
        Serial::println("Your ACPI controller might have latency");
    }

    INT_DISABLE;
    SHOW_INT_DISABLE;

    Serial::println("Power off failed. Hanging...");
    for (;;) {
        CPU_HALT;
        SHOW_CPU_HALT;
    }
}

void reboot() {
    uint8_t good = 0x02;
    while (good & 0x02) good = inb(0x64);
    Serial::println("Rebooted.");
    outb(0x64, 0xFE);
    
    Serial::println("Reboot failed. Hanging...");
    for (;;) {
        CPU_HALT;
        SHOW_CPU_HALT;
    }
}

}; // namespace Kernel::Hardware