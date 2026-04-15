#include <driver/timer.hpp>
#include <driver/acpi.hpp>
#include <kernel/serial.hpp>
#include <kernel/ports.hpp>
#include <cfg/asm.txx>

using namespace Arch;
using namespace Kernel;
using namespace Ports;

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
    static HPETRegisters* hpet = nullptr;
    static uint32_t femtoseconds_per_tick = 0;

    void timer_callback(Arch::x86::Registers *regs) {
        // TODO: maybe add more logic?
        ticks++;

        EOI_MASTER;
    }

    void init_hpet() {
        hpet = (HPETRegisters*)ACPI::get_hpet_address();

        femtoseconds_per_tick = hpet->capabilities >> 32;

        hpet->configuration |= 0x01; 
        
        Serial::println("HPET:\tTick period (femtosecond): {}", femtoseconds_per_tick);
    }

    uint64_t uptime_ns() {
        if (!hpet) return 0;
        
        uint64_t counter = hpet->main_counter;
        return (counter * femtoseconds_per_tick) / 1000000;
    }

    void sleep_ns(uint64_t nanoseconds) {
        uint64_t start = uptime_ns();
        while (uptime_ns() - start < nanoseconds) {
            CPU_PAUSE;
        }
    }

    ull_t get_ticks() { return ticks; }
}; // namespace Driver::Timer