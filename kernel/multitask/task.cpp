#include <kernel/task.hpp>
#include <kernel/memory.hpp>
#include <kernel/vconsole.hpp>

namespace Kernel::Multitask {
    using namespace Mem;

    static Task* current_task = nullptr;

    static void nothing() {
        // this function does literally nothing
    }

    void init() {
        create_task(nothing);
    }

    uint32_t schedule(uint32_t current_esp) {
        if (current_task == nullptr) return current_esp;

        current_task->esp = current_esp;
        current_task = current_task->next;
        
        return current_task->esp;
    }

    void create_task(void (*entry_point)()) {
        Task* new_task = (Task*)malloc(sizeof(Task));
        uint32_t* stack = (uint32_t*)malloc(4096); 
        uint32_t* ptr = (uint32_t*)((uint32_t)stack + 4096);

        *(--ptr) = 0x202; // EFLAGS
        *(--ptr) = 0x08; // CS
        *(--ptr) = (uint32_t)entry_point; // EIP

        
        *(--ptr) = 0; // err_code
        *(--ptr) = 32; // int_no


        for(int i = 0; i < 8; i++) *(--ptr) = 0; 

        *(--ptr) = 0x10; // ds

        new_task->esp = (uint32_t)ptr;

        if (current_task == nullptr) {
            new_task->next = new_task;
            current_task = new_task;
        } else {
            new_task->next = current_task->next;
            current_task->next = new_task;
        }
    }
}