#include <kernel/vga.hpp>
#include <kernel/ports.hpp>

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

char Vga::getc(int x, int y) {
    return video[width * y + x] & 0xFF;
}

void Vga::printxy(const char *str, int x, int y) {
    for (int i = 0; str[i] != 0; i++) {
        if (str[i] == '\n') {
            y++;
            x = 0;
        } else Vga::putc(i + x, y, str[i]);
    }
}
void Kernel::Vga::update_cursor(int x, int y) {
    uint16_t pos = y * width + x;

    Ports::outb(0x3D4, 0x0F);
    Ports::outb(0x3D5, (uint8_t)pos & 0xFF);

    Ports::outb(0x3D4, 0x0E);
    Ports::outb(0x3D5, (uint8_t)(pos >> 8) & 0xFF);
}