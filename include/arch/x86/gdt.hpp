#pragma once

#include <cstdint>
#include <cfg/flags.txx>

namespace Arch::x86 {
    struct GDTEntry {
        uint16_t limit_low;
        uint16_t base_low;
        uint8_t base_middle;
        uint8_t access;
        uint8_t granularity;
        uint8_t base_high;
    } PACK;

    struct GDTPointer {
        uint16_t limit;
        uint32_t base;
    } PACK;

    void gdt_init();

    enum GDTSelectors {
        KCODE = 0x08,
        KDATA = 0x10,
        UCODE = 0x18 | 3,
        UDATA = 0x20 | 3,
        TSS = 0x28
    };
} // namespace Arch::x86
