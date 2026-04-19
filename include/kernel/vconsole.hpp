#pragma once

#include <klib/string.hpp>
#include <arch/x86/idt.hpp>

namespace Kernel {
    void kclear();
    void kcolor(char color);
    void kputc(int x, int y, char ch);
    void kprint(const kstd::string& text);
    void kprintln(const kstd::string& text = "");
    void kpprint(const kstd::string& text, char color);
    void kcprint(const kstd::string& text, char color);
    void kinfo(const kstd::string& text);
    void kdebug(const kstd::string& text);
    void kwarn(const kstd::string& text);
    void kpanic(const kstd::string& text, const Arch::x86::Registers* regs);
    int get_cursor_x();
    int get_cursor_y();
    void get_cursor(int& x, int& y);
} // namespace Kernel