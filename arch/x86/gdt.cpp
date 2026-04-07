#include <arch/x86/gdt.hpp>

using namespace Arch;

static x86::GDTEntry gdt[3];
static x86::GDTPointer gdt_ptr;

extern "C" void gdt_load(uint32_t);

static void gdt_set_entry(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[num].base_low = base & 0xFFFF;
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    gdt[num].limit_low = limit & 0xFFFF;
    gdt[num].granularity = (limit >> 16) & 0x0F;
    gdt[num].granularity |= gran & 0xF0;

    gdt[num].access = access;
}

void x86::gdt_init() {
    gdt_ptr.limit = sizeof(GDTEntry) * 3 - 1;
    gdt_ptr.base = (uint32_t)&gdt;

    // Null descriptor
    gdt_set_entry(0, 0, 0, 0, 0);

    // Code segment
    gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    // Data segment
    gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    gdt_load((uint32_t)&gdt_ptr);
}
