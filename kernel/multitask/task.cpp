#include <arch/x86/tss.hpp>
#include <kernel/task.hpp>
#include <kernel/memory.hpp>
#include <kernel/vconsole.hpp>
#include <driver/timer.hpp>
#include <klib/mem.hpp>
#include <klib/fmt.hpp>
#include <cfg/asm.txx>

namespace Kernel::Multitask {
    using namespace Arch;
    using namespace Mem;
    using namespace kstd;

    static Task* current_task = nullptr;
    static uint32_t task_id_counter = 0;

    extern "C" void task_exit() {
        if (current_task != nullptr) {
            current_task->status = TASK_TERMINATED;
            kinfo(fmt("Task {} ('{}') terminated", current_task->id, current_task->name));
        }

        while (1) CPU_HALT;
    }

    extern "C" void wrapper_kernel() {
        if (current_task && current_task->entry_point) current_task->entry_point();
        task_exit();
    }

    extern "C" void wrapper_user() {
        if (current_task && current_task->entry_point) current_task->entry_point();
        asm volatile (
            "movl $1, %%eax\n"
            "movl $1488, %%ebx\n"
            "int $0x80"
            :
            :
            : "eax", "ebx"
        );
    }

    void init() {
        create_task(nullptr, "idle", false, 128);
    }

    Task* find_task_by_id(uint32_t id) {
        if (current_task == nullptr) return nullptr;
        
        Task* task = current_task;
        do {
            if (task->id == id) return task;
            task = task->next;
        } while (task != current_task);
        
        return nullptr;
    }

    void cleanup_terminated_task(Task* task) {
        if (task == nullptr || task->status != TASK_TERMINATED) return;
        
        // If this is the only task, can't remove it
        if (task->next == task) return;
        
        // Remove from list
        if (task->prev != nullptr) {
            task->prev->next = task->next;
        }
        if (task->next != nullptr) {
            task->next->prev = task->prev;
        }
        
        // If current task was removed, move to next
        if (current_task == task) {
            current_task = task->next;
        }
        
        // Free resources
        if (task->kernel_stack != nullptr) {
            free(task->kernel_stack);
        }
        if (task->user_stack != nullptr) {
            free(task->user_stack);
        }
        free(task);
    }

    uint32_t schedule(uint32_t current_esp) {
        if (current_task == nullptr) return current_esp;

        // Save current task's ESP
        current_task->esp = current_esp;

        // Try to find next runnable task (skip paused and terminated tasks)
        Task* start = current_task;
        Task* next = current_task->next;
        int attempts = 0;
        const int max_attempts = 1000; // Prevent infinite loop

        while (attempts++ < max_attempts) {
            if (next->status == TASK_SLEEPING) {
                if (Driver::Timer::ktime_ms() >= next->wake_at) next->status = TASK_RUNNING;
            }
            if (next->status == TASK_RUNNING) {
                current_task = next;
                return current_task->esp;
            } else if (next->status == TASK_TERMINATED) {
                // Clean up terminated task, but continue searching
                Task* to_cleanup = next;
                next = next->next;
                cleanup_terminated_task(to_cleanup);
                continue;
            }
            next = next->next;
        }

        x86::tss_entry.esp0 = current_task->kstack_top;

        // Fallback: just use current task (shouldn't happen)
        return current_task->esp;
    }

    uint32_t create_task(task_t entry_point, const char* name, bool user, uint32_t stack_size) {
        Task* new_task = (Task*)malloc(sizeof(Task));

        uint32_t* kstack = (uint32_t*)malloc(stack_size);
        new_task->kernel_stack = kstack;
        new_task->kstack_top = (uint32_t)kstack + stack_size;
        new_task->is_user = user;

        uint32_t* ustack = (uint32_t*)malloc(stack_size);
        new_task->user_stack = ustack;
        uint32_t* ptr = (uint32_t*)((uint32_t)ustack + stack_size);

        if (user) {
            *(--ptr) = 0x23;                // SS (user)
            *(--ptr) = (uint32_t)ptr + 4;   // ESP (user)
            *(--ptr) = 0x202;               // EFLAGS
            *(--ptr) = 0x1B;                // CS (user)
            *(--ptr) = (uint32_t)wrapper_user;
        } else {
            *(--ptr) = 0x202;               //  EFLAGS
            *(--ptr) = 0x08;                // Kernel Code
            *(--ptr) = (uint32_t)wrapper_kernel;
        }

        *(--ptr) = 0; // err_code
        *(--ptr) = 32; // int_no
        for (int i = 0; i < 8; i++) *(--ptr) = 0; // pusha

        // DS
        *(--ptr) = user ? 0x23 : 0x10;

        new_task->esp = (uint32_t)ptr;

        new_task->id = task_id_counter++;
        new_task->status = TASK_RUNNING;
        new_task->name = name ? name : "unnamed";
        new_task->next = new_task;
        new_task->prev = new_task;
        new_task->entry_point = entry_point;

        if (current_task == nullptr) {
            new_task->next = new_task;
            new_task->prev = new_task;
            current_task = new_task;
        } else {
            // Insert after current task
            new_task->next = current_task->next;
            new_task->prev = current_task;
            current_task->next->prev = new_task;
            current_task->next = new_task;
        }

        kinfo(fmt("Created task {} ('{}') with kstack at %x, ustack at %x", new_task->id, name, (uint32_t)kstack, (uint32_t)ustack));

        return new_task->id;
    }

    bool kill_task(uint32_t id) {
        Task* task = find_task_by_id(id);
        if (task == nullptr) {
            kwarn(fmt("kill_task: Task {} not found", id));
            return false;
        }

        if (task == current_task && task->next == task) {
            kwarn("Cannot kill the only remaining task");
            return false;
        }

        task->status = TASK_TERMINATED;
        kinfo(fmt("Task {} ('{}') marked for termination", id, task->name));
        return true;
    }

    bool pause_task(uint32_t id) {
        Task* task = find_task_by_id(id);
        if (task == nullptr) {
            kwarn(fmt("pause_task: Task {} not found", id));
            return false;
        }

        task->status = TASK_PAUSED;
        kinfo(fmt("Task {} ('{}') paused", id, task->name));
        return true;
    }

    bool resume_task(uint32_t id) {
        Task* task = find_task_by_id(id);
        if (task == nullptr) {
            kwarn(fmt("resume_task: Task {} not found", id));
            return false;
        }

        if (task->status != TASK_PAUSED) {
            kwarn(fmt("Task {} is not paused", id));
            return false;
        }

        task->status = TASK_RUNNING;
        kinfo(fmt("Task {} (%s) resumed", id, task->name));
        return true;
    }

    void sleep_task(uint32_t task_id, uint32_t ms) {
        Task* task = find_task_by_id(task_id);

        task->wake_at = Driver::Timer::ktime_ms() + ms;
        task->status = TASK_SLEEPING;
    }

    uint32_t get_current_task_id() {
        if (current_task == nullptr) return 0;
        return current_task->id;
    }

    void list_tasks() {
        if (current_task == nullptr) {
            kprintln("No tasks");
            return;
        }

        Task* task = current_task;
        int count = 0;
        
        do {
            const char* status_str = "RUNNING";
            if (task->status == TASK_PAUSED) status_str = "PAUSED";
            else if (task->status == TASK_TERMINATED) status_str = "TERMINATED";
            
            string marker = (task == current_task) ? " <-- current" : "";
            kprintln(fmt("[{}] '{}' (id={}, esp=%x, status={}){}", count++, task->name, task->id, task->esp, status_str, marker));
            
            task = task->next;
        } while (task != current_task);
    }

}; // namespace Kernel::Multitask