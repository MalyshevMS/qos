#include <cfg/entry.txx>
#include <cfg/asm.txx>
#include <kernel/serial.hpp>
#include <kernel/memory.hpp>
#include <kernel/vga.hpp>
#include <driver/keyboard.hpp>

#include <arch/x86/gdt.hpp>
#include <arch/x86/idt.hpp>
#include <arch/x86/pic.hpp>

#include <klib/string.hpp>

#include <klib/fmt.hpp>

using namespace Kernel;
using namespace Mem;
using namespace Arch;
using namespace kstd;

int x = 0; 
int y = 0;
int promt_y = 0;

FMT(void printf) {
    string str = fmt(format, args...);
    Vga::printxy(str.c_str(), x, y);
    x += str.size();

    if (x > Vga::width) {
        x = 1;
        y++;
    }

    if (y > Vga::height) {
        char syms[Vga::width][Vga::height];

        for (int i = 0; i < Vga::width; i++) {
            for (int j = 0; j < Vga::height - 1; j++) {
                syms[i][j] = Vga::getc(i, j + 1);
            }
        }

        Vga::clear();

        for (int i = 0; i < Vga::width; i++) {
            for (int j = 0; j < Vga::height; j++) {
                Vga::putc(i, j, syms[i][j]);
            }
        }

        y = Vga::height;
    }
}

KERNEL_ENTRY
void kernel_main() {
    x86::gdt_init();
    meminit();
    Serial::init();
    Keyboard::init();
    x86::pic_remap();
    x86::idt_init();

    x86::irq_register_handler(1, (x86::irq_handler_t)&Keyboard::keyboard_callback);
    x86::pic_unmask_irq(1);

    INT_ENABLE;

    Vga::clear();
    
    const string promt = "(kernel)> ";

    printf("{}", promt);

    string input;

    while (1) {
        if (Keyboard::has_data()) {
            auto sc = Keyboard::getscan();
            auto ch = Keyboard::scantochar(sc);

            if (ch != 0 && (ch != '\n' && ch != '\b')) {
                printf("{}", ch);
                input += ch;
                Serial::println("Current input: '{}'", input);
            }

            if (sc == Keyboard::SCANCODE_BACKSPACE) {
                auto xl = x;
                auto yl = y;

                x--;
                if (x == promt.size() - 1 && promt_y == y) x++;
                if (x < 0) {
                    x = Vga::width - 1;
                    y--;
                }
                Vga::putc(x, y, 0);
                if (xl != x && yl != y) input = input.substr(0, input.size() - 2);
            }

            if (sc == Keyboard::SCANCODE_ENTER) {
                Serial::println("Executing command: {}", input);
                // Command execution here
                if (input == "help") {
                    printf("No commands yet.\n");
                }

                printf("{}", promt);
                promt_y = y;
                input = "";
            }
        }

        Vga::update_cursor(x, y);
        CPU_HALT;
    }
}