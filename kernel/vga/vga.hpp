#pragma once


namespace Vga {
    extern unsigned short* video;
    constexpr unsigned short width = 80;
    constexpr unsigned short height = 25;
    extern char color;

    void clear();
    
    void putc(int x, int y, char c);
}