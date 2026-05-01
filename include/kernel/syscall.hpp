#pragma once

#include <arch/x86/idt.hpp>
#include <cstdint>
#include <cfg/syscall.txx>

namespace Kernel {
namespace Syscall {
    void sys_exit(SYSCALL_ARGS);
}; // namespace Syscall
}; // namespace Kernel