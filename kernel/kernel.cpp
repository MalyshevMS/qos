#include "vga/vga.hpp"
#include "serial/serial.hpp"
#include "config.txx"

KERNEL_ENTRY
void kernel_main() {
    Vga::clear();
    Serial::init();

    auto text = "Hello from VGA!";
    for (int i = 0; text[i] != 0; i++) {
        Vga::putc(i, 0, text[i]);
    }

    Serial::write("Hello from serial!\n");

    while (1) {
        asm volatile("hlt");
    }
}