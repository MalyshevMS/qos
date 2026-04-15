#include <driver/timer.hpp>
#include <kernel/serial.hpp>
#include <kernel/ports.hpp>
#include <cfg/asm.txx>

using namespace Arch;
using namespace Kernel;
using namespace Ports;

namespace Driver::Timer {
    ull_t ticks;

    void timer_callback(Arch::x86::Registers *regs) {
        // TODO: maybe add more logic?
        ticks++;

        EOI_MASTER;
    }

    ull_t get_ticks() { return ticks; }
}; // namespace Driver::Timer