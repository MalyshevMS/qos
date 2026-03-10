#include <cfg/entry.txx>
#include <kernel/serial.hpp>
#include <kernel/memory.hpp>
#include <kernel/vga.hpp>

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
    Vga::clear();
    
    x86::pic_remap();
    x86::idt_init();

    x86::idt_set_gate(33, (uint32_t)irq1_stub);
    x86::pic_unmask_irq(1);

    asm volatile("sti");

    Serial::write("Kernel ready.\n");

    while (1) {
        asm volatile("hlt");
    }
}