#include <arch/x86/irq.hpp>
#include <kernel/ports.hpp>
#include <kernel/serial.hpp>

using namespace Kernel;

extern "C" void irq1_handler() {
    uint8_t scancode = Ports::inb(0x60);

    Serial::write("KEY: ");
    Serial::write_hex(scancode);
    Serial::write_char('\n');

    Ports::outb(0x20, 0x20);
}