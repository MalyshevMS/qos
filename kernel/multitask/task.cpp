#include <arch/x86/tss.hpp>
#include <kernel/task.hpp>
#include <kernel/memory.hpp>
#include <kernel/vconsole.hpp>
#include <klib/mem.hpp>
#include <klib/fmt.hpp>
#include <driver/timer.hpp>
#include <cfg/asm.txx>

namespace Kernel::Multitask {
    using namespace Arch;
    using namespace Mem;
    using namespace kstd;

    static Task* current_task = nullptr;
    static uint32_t task_id_counter = 0;
    static const uint32_t TASK_STACK_SIZE = 4096;

    extern "C" void task_exit() {
        if (current_task != nullptr) {
            current_task->status = TASK_TERMINATED;
            kinfo(fmt("Task {} ('{}') terminated", current_task->id, current_task->name));
        }

        while (1) CPU_HALT;
    }

    void init() {
        create_task(nullptr, "idle");
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
        if (task->stack_base != nullptr) {
            free(task->stack_base);
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
            if (next->status == TASK_RUNNING) {
                if (next->is_sleeping) {
                    if (Driver::Timer::ktime() >= next->sleep_until) next->is_sleeping = false; // OTKA3 OT CHA
                    else continue;
                }

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

    uint32_t create_task(void (*entry_point)(), const char* name, bool user) {
        Task* new_task = (Task*)malloc(sizeof(Task));

        uint32_t* kstack = (uint32_t*)malloc(TASK_STACK_SIZE);
        new_task->kernel_stack = kstack;
        new_task->kstack_top = (uint32_t)kstack + TASK_STACK_SIZE;
        new_task->is_user = user;

        uint32_t* ustack = (uint32_t*)malloc(TASK_STACK_SIZE);
        new_task->stack_base = ustack;
        uint32_t* ptr = (uint32_t*)((uint32_t)ustack + TASK_STACK_SIZE);

        if (user) {
            *(--ptr) = 0x23;                // SS (user)
            *(--ptr) = (uint32_t)ptr + 4;   // ESP (user)
            *(--ptr) = 0x202;               // EFLAGS
            *(--ptr) = 0x1B;                // CS (user)
            *(--ptr) = (uint32_t)entry_point;
        } else {
            *(--ptr) = 0x202;               //  EFLAGS
            *(--ptr) = 0x08;                // Kernel Code
            *(--ptr) = (uint32_t)entry_point;
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

    void sleep_task(uint32_t task_id, uint64_t amount_ns) {
        auto task = find_task_by_id(task_id);
        task->sleep_until = Driver::Timer::ktime() + amount_ns;
        task->is_sleeping = true; // COH BPEDEH!
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
            kprintln(fmt("[{}] '{}' (id={}, esp=%x, status={}, is_sleeping:{}){}", count++, task->name, task->id, task->esp, status_str, (int)(task->is_sleeping), marker));
            
            task = task->next;
        } while (task != current_task);
    }

}; // namespace Kernel::Multitask