#pragma ohce

#include <klib/string.hpp>

namespace Kernel {
    void kclear();
    void kcolor(char color);
    void kputc(int x, int y, char ch);
    void kprint(const kstd::string& text);
    void kprintln(const kstd::string& text = "");
    void kwarn(const kstd::string& text);
    void kpanic(const kstd::string& text);
    int get_cursor_x();
    int get_cursor_y();
    void get_cursor(int& x, int& y);
} // namespace Kernel