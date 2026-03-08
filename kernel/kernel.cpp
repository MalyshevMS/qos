#include "vga/vga.hpp"
#include "serial/serial.hpp"
#include "mem/heap.hpp"
#include "mem/new.txx"
#include "config.txx"

using namespace Mem;

KERNEL_ENTRY
void kernel_main() {
    Vga::clear();
    Serial::init();

    auto x = new int;
    Serial::write_char('\n');

    while (1) {
        asm volatile("hlt");
    }
}