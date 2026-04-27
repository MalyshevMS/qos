#include <arch/x86/gdt.hpp>

namespace Arch::x86 {

static GDTEntry gdt[6];
static GDTPointer gdt_ptr;

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

void gdt_init() {
    gdt_ptr.limit = sizeof(GDTEntry) * 3 - 1;
    gdt_ptr.base = (uint32_t)&gdt;

    // Null descriptor
    gdt_set_entry(0, 0, 0, 0, 0);

    // Kernel Code segment
    gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    // Kernel Data segment
    gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    // User Code segment
    gdt_set_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);

    // User Data segment
    gdt_set_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    // TSS (not implemented yet)
    gdt_set_entry(5, 0, 0, 0, 0);

    gdt_load((uint32_t)&gdt_ptr);
}

} // namespace Arch::x86