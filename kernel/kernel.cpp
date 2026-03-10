#include "vga/vga.hpp"
#include "serial/serial.hpp"
#include "mem/heap.hpp"
#include "mem/new.txx"
#include "str/strlen.hpp"
#include "config.txx"

using namespace Mem;

KERNEL_ENTRY
void kernel_main() {
    Vga::clear();
    Serial::init();

    auto text = new char[] { "Klyde 41.71" };
    auto len = strlen(text);
    for (int y = 0; y < Vga::height; y++) {
        for (int i = 0; i < len; i++) {
            for (int j = 0; j < Vga::width / len; j++) {
                Vga::putc(i + j * len + j, y, text[i]);
            }
        }
    }

    delete[] text;

    Serial::write("Klyde 41.71\n");

    while (1) {
        asm volatile("hlt");
    }
}