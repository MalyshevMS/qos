#include <arch/x86/exceptions.hpp>
#include <kernel/vconsole.hpp>
#include <klib/fmt.hpp>

namespace Arch::x86::Exceptions {

    using namespace Kernel;
    using namespace kstd;

    void division_error(Registers* regs) {
        kpanic(fmt("Division by zero error at EIP: %x", regs->eip), regs);
    }

    void invalid_opcode(Registers* regs) {
        kpanic(fmt("Invalid Opcode error at EIP: %x", regs->eip), regs);
    }

    void double_fault(Registers* regs) {
        kpanic(fmt("Double Fault error at EIP: %x", regs->eip), regs);
    }

    void general_protection_fault(Registers* regs) {
        kpanic(fmt("General Protection Fault error at EIP: %x", regs->eip), regs);
    }

}; // namespace Arch::x86::Exceptions