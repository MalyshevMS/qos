#include <kernel/vconsole.hpp>
#include <kernel/vga.hpp>
#include <kernel/serial.hpp>
#include <driver/timer.hpp>
#include <cfg/cfg.txx>
#include <cfg/asm.txx>
#include <cfg/flags.txx>


namespace Kernel {

    using namespace kstd;
    using namespace Arch;

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

        int fg_base = 30;
        int fg_bright = (fg_color & 0x08) ? 60 : 0;
        int fg_ansi = vga_to_ansi_idx[fg_color & 0x07];
        string fg_part = to_string(fg_base + fg_bright + fg_ansi);

        string bg_part;
        if (bg_color == 0) {
            bg_part = "49";
        } else {
            int bg_base = 40;
            int bg_bright = (bg_color & 0x08) ? 60 : 0;
            int bg_ansi = vga_to_ansi_idx[bg_color & 0x07];
            bg_part = to_string(bg_base + bg_bright + bg_ansi);
        }

        return "\033[" + fg_part + ";" + bg_part + "m";
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

    void kcprint(const string& text, char color) {
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
                int padding_right = width - line_len;
                
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
        Serial::println();
        Serial::print(ANSI_CLEAR);

        Vga::color = col;
    }

    void kwarn(const string &text) {
        kpprint(text, 0xE0);
    }

    void kinfo(const string &text) {
        auto ktime = Driver::Timer::ktime_ms();
        auto str = fmt("Kernel message [{}.{}]: {}", ktime / 1'000, ktime % 1'000, text);
        kcprint(str, 0x03);
    }

    void kdebug(const string &text) {
        kpprint(text, 0x2F);
    }

    void kpanic(const string &text, const Arch::x86::Registers* regs) {
        SHOW_INT_DISABLE;
        INT_DISABLE;

        auto col = 0x4F;
        kpprint("Kernel panic!", col);
        kcprint("\n<==== start trace ====>\nMessage:", col);
        kcprint(text, col);

        if (regs == nullptr) {
            regs = Driver::Timer::get_last_registers();
        }

        auto regs_str = fmt(
            "ds: %x, edi: %x, esi: %x,\n"
            "ebp: %x, esp: %x, edx: %x,\n"
            "ecx: %x, eax: %x,\n"
            "int_no: %x, err_code: %x,\n"
            "eip: %x, cs: %x, eflags: %x,\n"
            "useresp: %x, ss: %x",

            regs->ds,       regs->edi,      regs->esi,
            regs->ebp,      regs->esp,      regs->edx,
            regs->ecx,      regs->eax,
            regs->int_no,   regs->err_code,
            regs->eip,      regs->cs,       regs->eflags,
            regs->useresp,  regs->ss
        );

        kcprint("Regisers:", col);
        kcprint(regs_str, col);
        kcprint("<==== end trace ====>", col);
        Serial::println(ANSI_CLEAR);

        SHOW_CPU_HALT;
        for (;;) CPU_HALT;
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