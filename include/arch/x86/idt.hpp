#pragma once

#include <cstdint>

namespace Arch {
namespace x86 {
    struct IDTEntry {
        uint16_t offset_low;
        uint16_t selector;
        uint8_t zero;
        uint8_t type_attr;
        uint16_t offset_high;
    } __attribute__((packed));

    struct IDTPointer {
        uint16_t limit;
        uint32_t base;
    } __attribute__((packed));

    struct Registers {
        uint32_t ds;
        uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
        uint32_t int_no, err_code;
        uint32_t eip, cs, eflags, useresp, ss;
    } __attribute__((packed));

    typedef void (*irq_handler_t)(Registers*);

    void idt_init();
    void idt_set_gate(int n, uint32_t handler);

    void irq_register_handler(int irq, irq_handler_t handler);
} // namespace x86
} // namespace Arch