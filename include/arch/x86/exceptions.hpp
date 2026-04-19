#pragma once

#include <arch/x86/idt.hpp>

namespace Arch {
namespace x86 {
namespace Exceptions {
    void division_error(Registers* regs);
    void invalid_opcode(Registers* regs);
    void double_fault(Registers* regs);
    void general_protection_fault(Registers* regs);
}; // namespace Exceptions
}; // namespace x86
}; // namespace Arch