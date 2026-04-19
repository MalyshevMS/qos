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
#include <driver/pci.hpp>
#include <driver/disk.hpp>

#include <arch/x86/gdt.hpp>
#include <arch/x86/idt.hpp>
#include <arch/x86/pic.hpp>
#include <arch/x86/exceptions.hpp>

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

    Vga::color = 0x0F;
    kclear();

    Serial::init();
    Keyboard::init();
    ACPI::init();
    Timer::init_hpet();
    x86::pic_remap();
    x86::idt_init();
    PCI::init();
    Disk::init();
    
    x86::irq_register_handler(0, (x86::handler_t)&Timer::timer_callback);
    x86::pic_unmask_irq(0);

    x86::irq_register_handler(1, (x86::handler_t)&Keyboard::keyboard_callback);
    x86::pic_unmask_irq(1);

    x86::exception_register_handler(0x00, (x86::handler_t)&x86::Exceptions::division_error);
    x86::exception_register_handler(0x06, (x86::handler_t)&x86::Exceptions::invalid_opcode);
    x86::exception_register_handler(0x08, (x86::handler_t)&x86::Exceptions::double_fault);
    x86::exception_register_handler(0x0D, (x86::handler_t)&x86::Exceptions::general_protection_fault);

    kinfo(fmt("Kernel time: {} nanoseconds", Timer::ktime()));
    kinfo(fmt("Zero uptime: {} nanoseconds", Timer::uptime_ns()));

    INT_ENABLE;
    SHOW_INT_ENABLE;

    Console::init();
    Console::run();

    kwarn("You shouldn't have reach this far. Hanging...");
    
    while (1) {
        CPU_HALT;
        SHOW_CPU_HALT;
    }
}