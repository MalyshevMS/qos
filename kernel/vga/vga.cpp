#include "vga.hpp"

char Vga::color = 0x1F;

void Vga::clear() {
    for (int i = 0; i < width * height; i++) {
        VGA[i] = color << 8;
    }
}

void Vga::putc(int x, int y, char c) {
    VGA[width * y + x] = (color << 8) | c;
}
