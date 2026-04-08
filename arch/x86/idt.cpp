#include <arch/x86/idt.hpp>
#include <arch/x86/pic.hpp>

using namespace Arch;

static x86::IDTEntry idt[256];
static x86::IDTPointer idt_ptr;
static x86::irq_handler_t irq_handlers[16] = {0};

extern "C" void idt_load(uint32_t);
extern "C" uint32_t irq_stub_table[];

void x86::idt_set_gate(int n, uint32_t handler) {
    idt[n].offset_low = handler & 0xFFFF;
    idt[n].selector = 0x08;
    idt[n].zero = 0;
    idt[n].type_attr = 0x8E;
    idt[n].offset_high = (handler >> 16) & 0xFFFF;
}

void x86::irq_register_handler(int irq, irq_handler_t handler) {
    irq_handlers[irq] = handler;
}

extern "C" void irq_common_handler(x86::Registers* regs) {
    int irq = regs->int_no - 32;

    if (irq_handlers[irq] != nullptr) irq_handlers[irq](regs);

    pic_send_eoi(irq);
}

void x86::idt_init() {
    idt_ptr.limit = sizeof(IDTEntry) * 256 - 1;
    idt_ptr.base = (uint32_t)&idt;

    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0);
    }

    for (int i = 0; i < 16; i++) {
        idt_set_gate(32 + i, irq_stub_table[i]);
    }

    idt_load((uint32_t)&idt_ptr);
}