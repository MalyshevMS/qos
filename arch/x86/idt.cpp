#include <arch/x86/idt.hpp>

using namespace Arch;

static x86::IDTEntry idt[256];
static x86::IDTPointer idt_ptr;

extern "C" void idt_load(uint32_t);

void x86::idt_set_gate(int n, uint32_t handler) {
    idt[n].offset_low = handler & 0xFFFF;
    idt[n].selector = 0x08;
    idt[n].zero = 0;
    idt[n].type_attr = 0x8E;
    idt[n].offset_high = (handler >> 16) & 0xFFFF;
}

void x86::idt_init() {
    idt_ptr.limit = sizeof(IDTEntry) * 256 - 1;
    idt_ptr.base = (uint32_t)&idt;

    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0);
    }

    idt_load((uint32_t)&idt_ptr);
}