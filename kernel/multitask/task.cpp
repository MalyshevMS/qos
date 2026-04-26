#include <kernel/task.hpp>
#include <kernel/memory.hpp>
#include <kernel/vconsole.hpp>
#include <klib/mem.hpp>
#include <klib/fmt.hpp>
#include <cfg/asm.txx>

namespace Kernel::Multitask {
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

        // Fallback: just use current task (shouldn't happen)
        return current_task->esp;
    }

    uint32_t create_task(void (*entry_point)(), string name) {
        Task* new_task = (Task*)malloc(sizeof(Task));
        if (new_task == nullptr) {
            kpanic("Failed to allocate task structure");
            return 0;
        }

        uint32_t* stack = (uint32_t*)malloc(TASK_STACK_SIZE);
        if (stack == nullptr) {
            free(new_task);
            kpanic("Failed to allocate task stack");
            return 0;
        }

        // Calculate stack pointer
        uint32_t* ptr = (uint32_t*)((uint32_t)stack + TASK_STACK_SIZE);
        
        // Return address for the task function (will be on stack after iret)
        *(--ptr) = (uint32_t)&task_exit;  // Return address when task function returns

        // Set up interrupt frame for iret
        *(--ptr) = 0x202;           // EFLAGS (interrupts enabled)
        *(--ptr) = 0x08;            // CS
        *(--ptr) = (uint32_t)entry_point;  // EIP - entry point

        // Exception/IRQ frame fields
        *(--ptr) = 0;               // err_code
        *(--ptr) = 32;              // int_no

        // General purpose registers (will be popped by popa in irq_handler)
        for (int i = 0; i < 8; i++) *(--ptr) = 0;

        *(--ptr) = 0x10;            // ds

        new_task->id = task_id_counter++;
        new_task->esp = (uint32_t)ptr;
        new_task->stack_base = stack;
        new_task->status = TASK_RUNNING;
        new_task->name = name.c_str() ? name : "unnamed";
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

        kinfo(fmt("Created task {} ('{}') with stack at %x", new_task->id, name.c_str(), (uint32_t)stack));
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
        kprintln("=== Task List ===");
        
        do {
            const char* status_str = "RUNNING";
            if (task->status == TASK_PAUSED) status_str = "PAUSED";
            else if (task->status == TASK_TERMINATED) status_str = "TERMINATED";
            
            string marker = (task == current_task) ? " <-- current" : "";
            kprintln(fmt("[{}] '{}' (id={}, esp=%x, status={}){}", count++, task->name.c_str(), task->id, task->esp, status_str, marker));
            
            task = task->next;
        } while (task != current_task);
        
        kprintln("=== End Task List ===");
    }

}; // namespace Kernel::Multitask