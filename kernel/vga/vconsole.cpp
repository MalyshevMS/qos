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

    void kpprint(const string& text, char color) {
        auto col = Vga::color;
        Vga::color = color;
        
        int width = 80;
        
        if (cursor_x != 0) {
            print_char('\n');
        }
        
        size_t start = 0;
        
        while (start < text.size()) {
            size_t end = text.find('\n', start);
            if (end == string::npos) {
                end = text.size();
            }
            
            string line = text.substr(start, end - start);
            int line_len = line.size();
            
            if (line_len == 0 && end == text.size()) {
                break;
            }
            
            if (line_len <= width) {
                int padding_left = (width - line_len) / 2;
                int padding_right = width - padding_left - line_len;
                
                for (int i = 0; i < padding_left; i++) {
                    print_char(' ');
                }
                if (line_len > 0) {
                    kprint(line);
                }
                for (int i = 0; i < padding_right; i++) {
                    print_char(' ');
                }
            } else {
                kprint(line);
                if (cursor_x > 0) {
                    int remaining = width - cursor_x;
                    for (int i = 0; i < remaining; i++) {
                        print_char(' ');
                    }
                }
            }
            
            if (end < text.size()) {
                if (cursor_x != 0) {
                    print_char('\n');
                }
                start = end + 1;
            } else {
                break;
            }
        }
        
        Vga::color = col;
    }

    void kwarn(const string &text) {
        kpprint(text, 0xE0);
    }

    void kpanic(const string &text) {
        kpprint(text, 0xCF);
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