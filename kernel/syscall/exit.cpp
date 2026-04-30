#include <kernel/syscall.hpp>
#include <kernel/task.hpp>
#include <kernel/vconsole.hpp>
#include <klib/fmt.hpp>

namespace Kernel::Syscall {
    using namespace kstd;

    SYSCALL(exit) {
        auto task = Multitask::get_current_task_id();

        Multitask::kill_task(task);

        kinfo(fmt("Task {} exited via syscall with return code {}", task, regs->ebx));

        return Multitask::schedule((uint32_t)regs);
    }
}; // namespace Kernel::Syscall