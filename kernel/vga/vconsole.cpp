#include <kernel/vconsole.hpp>
#include <kernel/vga.hpp>
#include <kernel/serial.hpp>
#include <driver/timer.hpp>
#include <cfg/cfg.txx>
#include <cfg/asm.txx>
#include <cfg/flags.txx>

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

    static string vga_to_ansi(unsigned char attr) {
        static const int vga_to_ansi_idx[] = {0, 4, 2, 6, 1, 5, 3, 7};

        unsigned char fg_color = attr & 0x0F;
        unsigned char bg_color = (attr >> 4) & 0x0F;

        auto format_part = [](unsigned char color, bool is_bg) -> string {
            int base = is_bg ? 40 : 30;
            int bright = (color & 0x08) ? 60 : 0;
            int ansi_idx = vga_to_ansi_idx[color & 0x07];
            char buff[12];
            itoa(base + bright + ansi_idx, buff);
            return buff;
        };

        return "\033[" + format_part(fg_color, false) + ";" + format_part(bg_color, true) + "m";
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
        
        Serial::print(vga_to_ansi(Vga::color));
        Serial::print(text);
        Serial::print(ANSI_CLEAR);
        Serial::println();

        Vga::color = col;
    }

    void kwarn(const string &text) {
        kpprint(text, 0xE0);
    }

    void kinfo(const string &text) {
        auto ktime = Driver::Timer::ktime_ms();
        auto col = Vga::color;
        auto str = fmt("Kernel message [{}.{}]: {}", ktime / 1'000, ktime % 1'000, text);
        Vga::color = 0x03;
        kprintln(str);
        Serial::print(vga_to_ansi(Vga::color));
        Serial::print(str);
        Serial::print(ANSI_CLEAR);
        Serial::println();
        Vga::color = col;
    }

    void kdebug(const string &text) {
        kpprint(text, 0x2F);
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