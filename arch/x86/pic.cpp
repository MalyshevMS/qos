#include <arch/x86/pic.hpp>
#include <kernel/ports.hpp>

using namespace Kernel::Ports;
using namespace Arch;

// PIC ports
#define PIC1_CMD    0x20
#define PIC1_DATA   0x21
#define PIC2_CMD    0xA0
#define PIC2_DATA   0xA1
#define PIC_EOI     0x20

void x86::pic_remap() {
    // ICW1: Initialize 8259A with ICW1
    outb(PIC1_CMD, 0x11);
    outb(PIC2_CMD, 0x11);

    // ICW2: Set interrupt vector offsets
    outb(PIC1_DATA, 0x20);  // Master PIC vectors: 0x20-0x27 (IRQ0-7)
    outb(PIC2_DATA, 0x28);  // Slave PIC vectors: 0x28-0x2F (IRQ8-15)

    // ICW3: Configure cascading
    outb(PIC1_DATA, 0x04);  // Master has slave at IRQ2
    outb(PIC2_DATA, 0x02);  // Slave is on IRQ2

    // ICW4: Enable 8086 mode
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    // OCW1: Mask all interrupts initially
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

void x86::pic_unmask_irq(int irq) {
    uint16_t port = (irq < 8) ? PIC1_DATA : PIC2_DATA;
    int bit = (irq < 8) ? irq : (irq - 8);
    
    uint8_t mask = inb(port);
    mask &= ~(1 << bit);
    outb(port, mask);
}

extern "C" void pic_send_eoi(int irq) {
    if (irq >= 8) {
        outb(PIC2_CMD, PIC_EOI);
    }
    outb(PIC1_CMD, PIC_EOI);
}