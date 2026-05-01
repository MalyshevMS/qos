#include <kernel/syscall.hpp>
#include <kernel/vconsole.hpp>
#include <kernel/task.hpp>
#include <klib/fmt.hpp>

namespace Kernel::Syscall {
    using namespace kstd;

    void sys_print(SYSCALL_ARGS) {
        kprint((const char*)regs->ebx);
    }
}; // namespace Kernel::Syscall