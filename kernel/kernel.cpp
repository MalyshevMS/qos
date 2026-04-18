#include <cfg/flags.txx>
#include <cfg/asm.txx>

#include <kernel/serial.hpp>
#include <kernel/memory.hpp>
#include <kernel/vga.hpp>
#include <kernel/console.hpp>
#include <kernel/power.hpp>
#include <kernel/vconsole.hpp>

#include <driver/keyboard.hpp>
#include <driver/timer.hpp>
#include <driver/acpi.hpp>

#include <arch/x86/gdt.hpp>
#include <arch/x86/idt.hpp>
#include <arch/x86/pic.hpp>

#include <klib/string.hpp>
#include <klib/fmt.hpp>

using namespace Kernel;
using namespace Mem;
using namespace Arch;
using namespace Driver;
using namespace kstd;

KERNEL_ENTRY
void kernel_main() {
    x86::gdt_init();
    meminit();
    Serial::init();
    Keyboard::init();
    x86::pic_remap();
    x86::idt_init();
    ACPI::init();
    
    x86::irq_register_handler(0, (x86::irq_handler_t)&Timer::timer_callback);
    x86::pic_unmask_irq(0);

    x86::irq_register_handler(1, (x86::irq_handler_t)&Keyboard::keyboard_callback);
    x86::pic_unmask_irq(1);

    Timer::init_hpet();

    Serial::println("Kernel time: {} nanoseconds", Timer::ktime());
    Serial::println("Zero uptime: {} nanoseconds", Timer::uptime_ns());

    INT_ENABLE;
    SHOW_INT_ENABLE;

    Vga::color = 0x0F;

    kclear();
    kinfo("test kinfo");

    Console::init();
    Console::run();

    Serial::println("You shouldn't have reach this far. Hanging...");
    
    while (1) {
        CPU_HALT;
        SHOW_CPU_HALT;
    }
}