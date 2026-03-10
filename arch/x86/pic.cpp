#include <arch/x86/pic.hpp>
#include <kernel/ports.hpp>

using namespace Kernel::Ports;
using namespace Arch;


void x86::pic_remap() {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    outb(0x21, 0x20);
    outb(0xA1, 0x28);

    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);
}

void x86::pic_unmask_irq(int irq) {
    uint16_t port = (irq < 8) ? 0x21 : 0xA1;
    int bit = (irq < 8) ? irq : (irq - 8);
    
    uint8_t mask = inb(port);
    mask &= ~(1 << bit);
    outb(port, mask);
}