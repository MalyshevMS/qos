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
    struct HPETTimerConfig {
        uint64_t config;
        uint64_t comparator;
        uint64_t fsb_interrupt_route;
        uint8_t  reserved[8];
    } PACK;

    struct HPETRegisters {
        uint64_t capabilities;
        uint64_t reserved0;
        uint64_t configuration;
        uint64_t reserved1;
        uint64_t interrupt_status;
        uint8_t  reserved2[0xF0 - 0x28];
        uint64_t main_counter;
        uint8_t  reserved3[0x100 - 0xF8];
        HPETTimerConfig timers[32];  // Timer configurations start at offset 0x100
    } PACK;

    static ull_t ticks;
    static x86::Registers* registers = nullptr;
    static HPETRegisters* hpet = nullptr;
    static uint32_t femtoseconds_per_tick = 0;
    static uint32_t ns_multiplier = 0;
    static uint64_t startup_ns = 0;
    static uint64_t cpu_freq = 0;

    void timer_callback(x86::Registers *regs) {
        ticks++;
    }

    void timer_init(uint32_t freq) {
        if (!hpet) {
            kwarn("HPET not initialized, cannot setup timer interrupt");
            return;
        }

        // Calculate the period between interrupts (in HPET ticks)
        uint64_t period = (1'000'000'000ULL / (uint64_t)freq) / (uint32_t)femtoseconds_per_tick;
        
        kinfo(fmt("HPET Timer init: freq={} Hz, period={} ticks", freq, period));

        // Configure Timer 0 for periodic mode with IRQ0
        uint64_t config = 0x0000;
        config |= (1 << 2);   //  Enable timer interrupt
        config |= (1 << 3);   // Set periodic mode
        config |= (0 << 5);   // Route to IRQ 0

        hpet->timers[0].config = config;
        hpet->timers[0].comparator = hpet->main_counter + period;

        kinfo("HPET Timer 0 configured for periodic interrupts");
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
        
        if (!hpet) {
            kpanic("HPET address is null");
            return;
        }

        // Get tick period from capabilities
        femtoseconds_per_tick = hpet->capabilities >> 32;
        
        kinfo(fmt("HPET: Tick period (femtosecond): {}", femtoseconds_per_tick));
        
        // Calculate nanoseconds multiplier for time calculations
        ns_multiplier = ((uint32_t)femtoseconds_per_tick / (uint32_t)1'000'000U);
        kinfo(fmt("HPET: Nanoseconds multiplier: {}", ns_multiplier));
        
        // Reset main counter to 0 before enabling
        hpet->main_counter = 0;
        
        // Enable HPET main counter
        hpet->configuration |= 0x01;
        
        kinfo("HPET: Main counter enabled");
        
        // Record startup time
        startup_ns = uptime_ns();
        kinfo(fmt("HPET: Startup time (nanoseconds): {}", startup_ns));

        // Calibrate CPU frequency using HPET
        stress_cpu(500);
        cpu_freq = calibrate_tsc();
        kinfo(fmt("HPET: CPU Frequency: {} Hz", cpu_freq));
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

    void test_hpet_counter() {
        if (!hpet) {
            kwarn("HPET not initialized");
            return;
        }
        
        kinfo("=== HPET Counter Test ===");
        kinfo(fmt("Tick period: {} fs", femtoseconds_per_tick));
        kinfo(fmt("NS multiplier: {} ns/tick", ns_multiplier));
        
        // Read counter multiple times and check it's increasing
        uint64_t readings[5];
        for (int i = 0; i < 5; i++) {
            readings[i] = hpet->main_counter;
        }
        
        bool increasing = true;
        for (int i = 1; i < 5; i++) {
            if (readings[i] <= readings[i-1]) {
                increasing = false;
                break;
            }
        }
        
        if (increasing) {
            kinfo("* HPET counter is working and incrementing");
            kinfo(fmt("  Counter progression: {} -> {} -> {} -> {} -> {}", 
                readings[0], readings[1], readings[2], readings[3], readings[4]));
        } else {
            kwarn("X HPET counter not incrementing properly!");
        }
    }

    void test_timer_interrupts() {
        if (!hpet) {
            kwarn("HPET not initialized");
            return;
        }
        
        kinfo("=== Timer Interrupt Test ===");
        
        uint64_t initial_ticks = ticks;
        uint64_t start_time = ktime_ms();
        
        // Wait for 1 second (1000 ms) and count interrupts
        while (ktime_ms() - start_time < 1000) {
            CPU_PAUSE;
        }
        
        uint64_t interrupt_count = ticks - initial_ticks;
        uint64_t elapsed_ms = ktime_ms() - start_time;
        
        kinfo(fmt("Interrupts received: {}", interrupt_count));
        kinfo(fmt("Elapsed time: {} ms", elapsed_ms));
        
        if (interrupt_count > 0) {
            kinfo(fmt("* Timer interrupts working (avg {} Hz)", 
                (interrupt_count * 1000) / elapsed_ms));
        } else {
            kwarn("X No timer interrupts detected!");
        }
    }

    void set_timer_frequency(uint32_t freq) {
        kinfo(fmt("Setting timer frequency to {} Hz", freq));
        timer_init(freq);
    }

    void set_max_precision() {
        // Set to 10 kHz for maximum precision (100 microsecond intervals)
        // Most systems support this easily
        set_timer_frequency(10'000);
        kinfo("Timer set to maximum precision (10 kHz, 100 microseconds intervals)");
    }

    void print_hpet_info() {
        if (!hpet) {
            kwarn("HPET not initialized");
            return;
        }
        
        kinfo("=== HPET Information ===");
        kinfo(fmt("Base address: %x", (uint32_t)hpet));
        kinfo(fmt("Capabilities: %x", hpet->capabilities));
        kinfo(fmt("Configuration: %x", hpet->configuration));
        kinfo(fmt("Interrupt status: %x", hpet->interrupt_status));
        kinfo(fmt("Main counter: {}", hpet->main_counter));
        kinfo(fmt("Tick period: {} fs", femtoseconds_per_tick));
        kinfo(fmt("NS multiplier: {}", ns_multiplier));
        kinfo(fmt("Total interrupts: {}", ticks));
        kinfo(fmt("Uptime: {} ms", ktime_ms()));
        
        // Timer 0 info
        kinfo(fmt("Timer 0 config: %x", hpet->timers[0].config));
        kinfo(fmt("Timer 0 comparator: {}", hpet->timers[0].comparator));
    }

    uint64_t measure_jitter(uint32_t samples) {
        if (!hpet || samples < 2) return 0;
        
        uint64_t max_delta = 0;
        uint64_t min_delta = ~0ULL;
        uint64_t prev = hpet->main_counter;
        
        for (uint32_t i = 0; i < samples; i++) {
            uint64_t curr = hpet->main_counter;
            uint64_t delta = curr - prev;
            
            if (delta > max_delta) max_delta = delta;
            if (delta < min_delta) min_delta = delta;
            
            prev = curr;
        }
        
        uint64_t jitter_ns = (max_delta - min_delta) * ns_multiplier;
        kinfo(fmt("Timer jitter: {} ns (min={}, max={} ticks)", 
            jitter_ns, min_delta, max_delta));
        
        return jitter_ns;
    }
}; // namespace Driver::Timer