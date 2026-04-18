#include <kernel/vconsole.hpp>
#include <kernel/vga.hpp>
#include <kernel/serial.hpp>
#include <driver/timer.hpp>
#include <cfg/cfg.txx>
#include <cfg/asm.txx>

using namespace kstd;

namespace Kernel {
    static int cursor_x = 0;
    static int cursor_y = 0;

#if CFG_VGA_TEXT_MODE == 1
    static void newline() {
        cursor_x = 0;
        cursor_y++;

        if (cursor_y >= Vga::height) {
            Vga::scroll_up();
            cursor_y = Vga::height - 1;
        }
    }

    static void print_char(char ch) {
        if (ch == '\n') {
            newline();
        } else if (ch == '\r') {
            cursor_x = 0;
        } else if (ch == '\b') {
            if (cursor_x > 0) {
                cursor_x--;
            }
        } else {
            kputc(cursor_x, cursor_y, ch);
            cursor_x++;

            if (cursor_x >= Vga::width) {
                newline();
            }
        }
        
        Vga::update_cursor(cursor_x, cursor_y);
    }
#endif

    void kclear() {
        Vga::clear();
        cursor_x = 0;
        cursor_y = 0;
    }

    void kcolor(char color) {
        Vga::color = color;
    }

    void kputc(int x, int y, char ch) {
        Vga::putc(x, y , ch);
    }

    void kprint(const string& text) {
        for (size_t i = 0; i < text.size(); i++) {
            print_char(text[i]);
        }
    }

    void kprintln(const string& text) {
        kprint(text);
        print_char('\n');
    }

    void kwarn(const string &text) {
        if (cursor_x != 0) kprintln();
        auto col = Vga::color;
        Vga::color = 0xE0;
        
        int width = 80;
        int padding = (width - text.size()) / 2;
        for (int i = 0; i < padding; i++) {
            print_char(' ');
        }
        kprint(text);
        for (int i = 0; i < width - padding - text.size(); i++) {
            print_char(' ');
        }
        
        Vga::color = col;
    }

    void kpanic(const string &text) {
        if (cursor_x != 0) kprintln();
        auto col = Vga::color;
        Vga::color = 0xCF;
        
        int width = 80;
        int padding = (width - text.size()) / 2;
        for (int i = 0; i < padding; i++) {
            print_char(' ');
        }
        kprint(text);
        for (int i = 0; i < width - padding - text.size(); i++) {
            print_char(' ');
        }
        kprintln();
        
        Vga::color = col;
        SHOW_INT_DISABLE;
        INT_DISABLE;
        SHOW_CPU_HALT;
        CPU_HALT;
        Driver::Timer::sleep(1);
    }

    int get_cursor_x() {
        return cursor_x;
    }

    int get_cursor_y() {
        return cursor_y;
    }

    void get_cursor(int& x, int& y) {
        x = cursor_x;
        y = cursor_y;
    }
}; // namespace Kernel