#include <kernel/syscall.hpp>
#include <kernel/vconsole.hpp>
#include <kernel/task.hpp>
#include <klib/fmt.hpp>

namespace Kernel::Syscall {
    using namespace kstd;

    void sys_exit(SYSCALL_ARGS) {
        auto task = Multitask::get_current_task_id();

        Multitask::kill_task(task);

        kinfo(fmt("Task {}: exited via syscall with code {}", task, regs->ebx));

        result_esp = Multitask::schedule((uint32_t)result_esp);
    }
}; // namespace Kernel::Syscall