#include <driver/timer.hpp>
#include <kernel/serial.hpp>

namespace Driver::Timer {
    ull_t ticks;

    void timer_callback(Arch::x86::Registers *regs) {
        // TODO: maybe add more logic?
        ticks++;
    }

    ull_t get_ticks() { return ticks; }
}; // namespace Driver::Timer