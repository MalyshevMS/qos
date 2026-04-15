#pragma once

#include <arch/x86/idt.hpp>
#include <cstdint>
#include <cfg/types.txx>

namespace Driver {
namespace Timer {
    void timer_callback(Arch::x86::Registers* regs);
    
    ull_t get_ticks(); // Note: returns time only since you registered callback

    void init_hpet();

    uint64_t uptime_ns();

    void sleep_ns(uint64_t nanoseconds);
    inline void sleep(uint64_t ms) { sleep_ns(ms * 1'000'000ull); }
}; // namespace Timer
}; // namespace Driver