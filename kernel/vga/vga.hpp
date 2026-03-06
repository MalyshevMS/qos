#pragma once


namespace Vga {
    #define VGA ((unsigned short*)0xB8000)
    constexpr unsigned short width = 80;
    constexpr unsigned short height = 25;
    extern char color;

    void clear();
    
    void putc(int x, int y, char c);
}