#include <arch/x86/idt.hpp>
#include <arch/x86/pic.hpp>
#include <kernel/vconsole.hpp>
#include <kernel/task.hpp>
#include <klib/fmt.hpp>
#include <driver/timer.hpp>

namespace Arch::x86 {

using namespace Kernel;

static IDTEntry idt[256];
static IDTPointer idt_ptr;
static handler_t irq_handlers[16] = {0};
static handler_t exception_handlers[32] = {0};

extern "C" void idt_load(uint32_t);
extern "C" void syscall_stub();
extern "C" uint32_t irq_stub_table[];
extern "C" uint32_t exception_stub_table[];

void idt_set_gate(int n, uint32_t handler, uint8_t dpl) {
    idt[n].offset_low = handler & 0xFFFF;
    idt[n].selector = 0x08;
    idt[n].zero = 0;
    idt[n].type_attr = 0x8E | ((dpl & 3) << 5);
    idt[n].offset_high = (handler >> 16) & 0xFFFF;
}

void irq_register_handler(int irq, handler_t handler) {
    irq_handlers[irq] = handler;
}

void exception_register_handler(int n, handler_t handler) {
    if (n < 32) exception_handlers[n] = handler;
}

extern "C" uint32_t irq_common_handler(Registers* regs) {
    int irq = regs->int_no - 32;

    if (irq_handlers[irq] != nullptr) {
        irq_handlers[irq](regs);
    }

    uint32_t result_esp = (uint32_t)regs;

    if (irq == 0) {
        result_esp = Multitask::schedule(result_esp);
    }

    pic_send_eoi(irq);
    
    return result_esp;
}

extern "C" void exception_common_handler(Registers* regs) {
    if (exception_handlers[regs->int_no] != nullptr) {
        exception_handlers[regs->int_no](regs);
    } else {
        kpanic("UNHANDLED EXCEPTION", regs);
    }
}

extern "C" uint32_t syscall_handler(Registers* regs) {
    auto num = regs->eax;
    auto tid = Multitask::get_current_task_id();
    uint32_t result_esp = (uint32_t)regs;

    if (num == 1) {
        kprint((const char*)regs->ebx);
    } else if (num == 2) {
        Multitask::sleep_task(tid, regs->ebx * 1'000'000);
    } else if (num == 3) {
        Multitask::kill_task(tid);

        kwarn(kstd::fmt("Task {}: exiting via syscall.", tid));

        result_esp = Multitask::schedule((uint32_t)result_esp);
    } else {
        kwarn("Unknown syscall.");
    }

    return result_esp;
}

void idt_init() {
    idt_ptr.limit = sizeof(IDTEntry) * 256 - 1;
    idt_ptr.base = (uint32_t)&idt;

    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0);
    }

    for (int i = 0; i < 32; i++) {
        idt_set_gate(i, exception_stub_table[i]);
    }

    for (int i = 0; i < 16; i++) {
        idt_set_gate(32 + i, irq_stub_table[i]);
    }

    idt_set_gate(0x80, (uint32_t)syscall_stub, 3);

    idt_load((uint32_t)&idt_ptr);
}

} // namespace Arch::x86