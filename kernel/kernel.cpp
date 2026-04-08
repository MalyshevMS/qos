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

KERNEL_ENTRY
void kernel_main() {
    x86::gdt_init();
    
    meminit();
    Serial::init();
    Keyboard::init();
    Vga::clear();
    
    x86::pic_remap();
    x86::idt_init();

    x86::irq_register_handler(1, (x86::irq_handler_t)&Keyboard::keyboard_callback);
    x86::pic_unmask_irq(1);

    INT_ENABLE;

    string text1 = "Welcome to my OS";
    string text2 = "(VGA text editor)";
    string hello_message = fmt::format("Hello, world {i}", 4171);

    Serial::write("Keyboard driver initialized. Press keys...\n");
    Vga::printxy(hello_message.c_str(), 0, 0);

    int x = hello_message.size();
    int y = 0;

    Vga::update_cursor(x, y);

    while (1) {
        if (Keyboard::has_data()) {
            auto sc = Keyboard::getscan();
            auto ch = Keyboard::scantochar(sc);

            if (ch != 0 && (ch != '\n' && ch != '\b')) {
                Vga::putc(x, y, ch);
                x++;
                if (x > Vga::width) {
                    x = 1;
                    y++;
                }
            }

            if (sc == Keyboard::SCANCODE_BACKSPACE) {
                Vga::putc(--x, y, 0);
                if (x < 0) {
                    x = Vga::width;
                    y--;

                    while (Vga::getc(x, y) == 0) x--;
                    x++;
                }

                if (y < 0) {
                    x = 0;
                    y = 0;
                }
            } else if (sc == Keyboard::SCANCODE_ENTER) {
                x = 0;
                y++;
            } else if (sc == Keyboard::SCANCODE_UP) {
                y--;
            } else if (sc == Keyboard::SCANCODE_DOWN) {
                y++;
            } else if (sc == Keyboard::SCANCODE_RIGHT) {
                x++;
            } else if (sc == Keyboard::SCANCODE_LEFT) {
                x--;
            }

            Vga::update_cursor(x, y);
        }
        
        CPU_HALT;
    }
}