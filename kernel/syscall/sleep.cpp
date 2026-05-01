#include <kernel/syscall.hpp>
#include <kernel/vconsole.hpp>
#include <kernel/task.hpp>
#include <klib/fmt.hpp>

namespace Kernel::Syscall {
    using namespace kstd;

    void sys_sleep(SYSCALL_ARGS) {
        auto task = Multitask::get_current_task_id();
        
        Multitask::sleep_task(task, regs->ebx);

        result_esp = Multitask::schedule((uint32_t)result_esp);
    }
}; // namespace Kernel::Syscall