#include <kernel/vga.hpp>

using namespace Kernel;

char Vga::color = 0x1F;
unsigned short* Vga::video = ((unsigned short*)0xB8000);

void Vga::clear() {
    for (int i = 0; i < width * height; i++) {
        video[i] = color << 8;
    }
}

void Vga::putc(int x, int y, char c) {
    video[width * y + x] = (color << 8) | c;
}

void Vga::print0(const char *str) {
    for (int i = 0; str[i] != 0; i++) {
        Vga::putc(i, 0, str[i]);
    }
}