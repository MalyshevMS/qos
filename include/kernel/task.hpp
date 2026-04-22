#pragma once
#include <arch/x86/idt.hpp>

namespace Kernel::Multitask {
    struct Task {
        uint32_t esp;
        Task* next;
    };

    void init();
    void create_task(void (*entry_point)());
    uint32_t schedule(uint32_t current_esp);
}