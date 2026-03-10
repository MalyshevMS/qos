#pragma once

#include <stdint.h>

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

    void idt_init();
    void idt_set_gate(int n, uint32_t handler);
} // namespace x86
} // namespace Arch