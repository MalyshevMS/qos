#pragma once
#include <arch/x86/idt.hpp>
#include <klib/string.hpp>

namespace Kernel::Multitask {
    enum TaskStatus {
        TASK_RUNNING,
        TASK_PAUSED,
        TASK_TERMINATED,
        TASK_SLEEPING
    };

    struct Task {
        uint32_t id;              // Unique task ID
        uint32_t esp;             // Stack pointer
        uint32_t* stack_base;     // Allocated stack base
        uint32_t* kernel_stack;   // Kernel stack pointer
        uint32_t kstack_top;      // Top of the kernel stack
        TaskStatus status;        // Current task status
        bool is_user;             // Is user or a kernel task
        const char* name;         // Task name for debugging
        uint64_t wake_at;         // When to wake up (if sleeping)
        Task* next;               // Next task in circular list
        Task* prev;               // Previous task for easier removal
    };

    void init();
    uint32_t create_task(void (*entry_point)(), const char* name = "task", bool user = false);
    uint32_t schedule(uint32_t current_esp);
    
    bool kill_task(uint32_t task_id);
    bool pause_task(uint32_t task_id);
    bool resume_task(uint32_t task_id);
    void sleep_task(uint32_t task_id, uint32_t ms);

    void list_tasks();
    uint32_t get_current_task_id();
    
    extern "C" void task_exit();
}