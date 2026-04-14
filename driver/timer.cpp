#include <driver/timer.hpp>
#include <kernel/serial.hpp>

namespace Driver::Timer {
    ull_t ktime;

    void timer_callback(Arch::x86::Registers *regs) {
        // TODO: maybe add more logic?
        ktime++;
    }

    ull_t get_time() { return ktime; }
}; // namespace Driver::Timer