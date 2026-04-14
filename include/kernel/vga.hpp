#pragma once

namespace Kernel {
namespace Vga {
    extern unsigned short* video;
    constexpr unsigned short width = 80;
    constexpr unsigned short height = 25;
    extern char color;

    void clear();
    void scroll_up();
    
    void putc(int x, int y, char c);
    char getc(int x, int y);
    void printxy(const char* str, int x, int y);
    void update_cursor(int x, int y);
} // namespace Vga
} // namespace Kernel