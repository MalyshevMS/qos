#pragma once

#include <arch/x86/idt.hpp>
#include <cstdint>
#include <cfg/types.txx>

namespace Driver {
namespace Timer {
    void timer_callback(Arch::x86::Registers* regs);
    const Arch::x86::Registers* get_last_registers();
    
    ull_t get_ticks(); // Note: returns time only since you registered callback

    void init_hpet();
    void timer_init(uint32_t freq);
    uint64_t calibrate_tsc();

    uint64_t uptime_ns();
    uint64_t ktime();
    inline uint64_t ktime_ms() { return ktime() / 1'000'000; }
    ull_t frequency();
    uint64_t tick_period();

    void sleep_ns(uint64_t nanoseconds);
    inline void sleep(uint64_t ms) { sleep_ns(ms * 1'000'000ull); }

    // Test and diagnostic functions
    void test_hpet_counter();
    void test_timer_interrupts();
    void set_timer_frequency(uint32_t freq);
    void set_max_precision();
    void print_hpet_info();
    uint64_t measure_jitter(uint32_t samples);
}; // namespace Timer
}; // namespace Driver