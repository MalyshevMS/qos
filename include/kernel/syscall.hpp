#pragma once

#include <cstdint>
#include <arch/x86/idt.hpp>
#include <cfg/syscall.txx>

namespace Kernel {
namespace Syscall {
    SYSCALL(exit);
    SYSCALL(print);
    SYSCALL(sleep);
}; // namespace Syscall
}; // namespace Kernel