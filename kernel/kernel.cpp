#include <cfg/entry.txx>
#include <cfg/asm.txx>
#include <kernel/serial.hpp>
#include <kernel/memory.hpp>
#include <kernel/vga.hpp>
#include <driver/keyboard.hpp>

#include <arch/x86/idt.hpp>
#include <arch/x86/pic.hpp>
#include <arch/x86/irq.hpp>

using namespace Kernel;
using namespace Mem;
using namespace Arch;

KERNEL_ENTRY
void kernel_main() {
    meminit();
    Serial::init();
    Keyboard::init();
    Vga::clear();
    
    x86::pic_remap();
    x86::idt_init();

    x86::idt_set_gate(33, (uint32_t)irq1_handler);
    x86::pic_unmask_irq(1);

    INT_ENABLE;

    Vga::print0("Keyboard driver initialized. Press keys...");

    while (1) {
        if (Keyboard::has_data()) {
            uint8_t sc = Keyboard::getscan();
            Serial::write("scancode: ");
            Serial::write_hex(sc);
            Serial::write("\n");
        }
        
        CPU_HALT;
    }
}