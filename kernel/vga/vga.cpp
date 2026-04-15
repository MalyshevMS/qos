#include <kernel/vga.hpp>
#include <kernel/ports.hpp>

namespace Kernel::Vga {

using namespace Ports;

char color = 0x1F;
unsigned short* video = ((unsigned short*)0xB8000);

void clear() {
    for (int i = 0; i < width * height; i++) {
        video[i] = color << 8;
    }
}

void scroll_up() {
    for (int i = 0; i < width * (height - 1); i++) {
        video[i] = video[i + width];
    }
    
    for (int i = width * (height - 1); i < width * height; i++) {
        video[i] = color << 8;
    }
}

void putc(int x, int y, char c) {
    video[width * y + x] = (color << 8) | c;
}

char getc(int x, int y) {
    return video[width * y + x] & 0xFF;
}

void printxy(const char *str, int x, int y) {
    for (int i = 0; str[i] != 0; i++) {
        if (str[i] == '\n') {
            y++;
            x = 0;
        } else putc(i + x, y, str[i]);
    }
}
void update_cursor(int x, int y) {
    uint16_t pos = y * width + x;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)pos & 0xFF);

    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)(pos >> 8) & 0xFF);
}

} // namespace Kernel::Vga