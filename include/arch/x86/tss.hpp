#pragma once

#include <cstdint>
#include <cfg/flags.txx>

namespace Arch {
namespace x86 {
    struct TSSEntry {
        uint32_t prev_tss; // Reserved
        uint32_t esp0;     // Kernel stack
        uint32_t ss0;      // Kernel stack segment
        uint32_t esp1, ss1, esp2, ss2; // Unused
        uint32_t cr3;
        uint32_t eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
        uint32_t es, cs, ss, ds, fs, gs;
        uint32_t ldt;
        uint16_t trap, iomap_base;
    } PACK;

    extern TSSEntry tss_entry;
}; // namespace x86
}; // namespace Arch