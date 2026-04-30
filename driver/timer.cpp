#include <driver/timer.hpp>
#include <driver/acpi.hpp>
#include <kernel/serial.hpp>
#include <kernel/ports.hpp>
#include <klib/mem.hpp>
#include <cfg/asm.txx>

using namespace Arch;
using namespace Kernel;
using namespace Ports;
using namespace kstd;

namespace Driver::Timer {
    struct HPETRegisters {
        uint64_t capabilities;
        uint64_t reserved0;
        uint64_t configuration;
        uint64_t reserved1;
        uint64_t interrupt_status;
        uint8_t  reserved2[0xF0 - 0x28];
        uint64_t main_counter;
        uint64_t reserved3;
    } PACK;

    static ull_t ticks;
    static x86::Registers* registers = nullptr;
    static HPETRegisters* hpet = nullptr;
    static uint32_t femtoseconds_per_tick = 0;
    static uint32_t ns_multiplier = 0;
    static uint64_t startup_ns = 0;
    static uint64_t cpu_freq = 0;

    void timer_callback(x86::Registers *regs) {
        // memcpy(registers, regs, sizeof(x86::Registers));
        ticks++;

        // EOI_MASTER;
    }

    void timer_init(uint32_t freq) {
        uint32_t divisor = 1193182 / freq;

        outb(0x43, 0x36);

        outb(0x40, (uint8_t)(divisor & 0xFF));
        outb(0x40, (uint8_t)(divisor >> 8));
    }

    const x86::Registers* get_last_registers() {
        return registers;
    }

    void stress_cpu(uint64_t ms) {
        kinfo("Stressing CPU to wake it up...");
        uint64_t start = uptime_ns();
        uint64_t duration = ms * 1000000ULL;

        volatile double dummy = 1.0;
        
        while (uptime_ns() - start < duration) {
            dummy = (dummy * 1.000001) + 0.000001;
        }
        kinfo("Stress done.");
    }

    void init_hpet() {
        hpet = (HPETRegisters*)ACPI::get_hpet_address();

        femtoseconds_per_tick = hpet->capabilities >> 32;

        hpet->configuration |= 0x01; 

        kinfo(fmt("HPET: Tick period (femtosecond): {}", femtoseconds_per_tick));
        
        ns_multiplier = ((uint32_t)femtoseconds_per_tick / (uint32_t)1'000'000U);
        kinfo(fmt("HPET: Nanoseconds multiplier: {}", ns_multiplier));
        
        startup_ns = uptime_ns();
        kinfo(fmt("HPET: Startup time (nanoseconds): {}", startup_ns));

        stress_cpu(500);
        cpu_freq = calibrate_tsc();
        kinfo(fmt("HPET: Frequency: {} Hz", cpu_freq));
    }

    uint64_t uptime_ns() {
        if (!hpet) return 0;
        
        return hpet->main_counter * ns_multiplier;
    }

    uint64_t ktime() {
        return uptime_ns() - startup_ns;
    }

    void sleep_ns(uint64_t nanoseconds) {
        uint64_t ticks_to_wait = nanoseconds / ns_multiplier;
        
        uint64_t start = hpet->main_counter;
        while (hpet->main_counter - start < ticks_to_wait) {
            CPU_PAUSE;
        }
    }

    ull_t frequency() {
        return cpu_freq;
    }

    uint64_t tick_period() {
        return femtoseconds_per_tick;
    }

    uint64_t calibrate_tsc() {
        uint64_t hpet_start = hpet->main_counter;
        uint64_t tsc_start = rdtsc();

        while (hpet->main_counter - hpet_start < 100'000);

        uint64_t tsc_end = rdtsc();
        uint64_t hpet_end = hpet->main_counter;

        uint64_t dtsc = tsc_end - tsc_start;
        uint64_t dhpet = hpet_end - hpet_start;

        uint64_t time_ns = (dhpet * (uint32_t)femtoseconds_per_tick) / 1'000'000ULL;
        
        uint64_t hz = (dtsc * 1'000'000'000ULL) / time_ns;

        return hz;
    }

    ull_t get_ticks() { return ticks; }
}; // namespace Driver::Timer