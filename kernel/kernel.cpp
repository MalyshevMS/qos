#include <cfg/entry.txx>
#include <kernel/serial.hpp>
#include <kernel/memory.hpp>
#include <kernel/vga.hpp>

using namespace Kernel;
using namespace Mem;

KERNEL_ENTRY
void kernel_main() {
    meminit();
    Serial::init();
    Vga::clear();

    auto text = new char[] { "Hello, VGA!" };
    for (int i = 0; text[i] != 0; i++) {
        Vga::putc(i, 0, text[i]);
    }

    Serial::write("Hello, serial!\n");

    while (1) {
        asm volatile("hlt");
    }
}