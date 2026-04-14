#pragma once

#include <arch/x86/idt.hpp>
#include <cstdint>
#include <cfg/types.txx>

namespace Driver {
namespace Timer {
    void timer_callback(Arch::x86::Registers* regs);
    
    ull_t get_time(); // Note: returns time only since you registered callback
}; // namespace Timer
}; // namespace Driver