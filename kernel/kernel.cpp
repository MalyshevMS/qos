#include <cfg/asm.txx>
#include <cfg/flags.txx>

#include <cstdint>
#include <kernel/console.hpp>
#include <kernel/fs.hpp>
#include <kernel/memory.hpp>
#include <kernel/power.hpp>
#include <kernel/serial.hpp>
#include <kernel/syscall.hpp>
#include <kernel/task.hpp>
#include <kernel/vconsole.hpp>
#include <kernel/vga.hpp>

#include <driver/acpi.hpp>
#include <driver/disk.hpp>
#include <driver/keyboard.hpp>
#include <driver/pci.hpp>
#include <driver/timer.hpp>

#include <arch/x86/exceptions.hpp>
#include <arch/x86/gdt.hpp>
#include <arch/x86/idt.hpp>
#include <arch/x86/pic.hpp>
#include <arch/x86/tss.hpp>

#include <klib/fmt.hpp>
#include <klib/map.hpp>
#include <klib/string.hpp>

#include <fs/ramfs.hpp>

using namespace Kernel;
using namespace Mem;
using namespace Arch;
using namespace Driver;
using namespace FS;
using namespace Syscall;
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
    Timer::timer_init(1'000); // Hz
    Timer::set_max_precision();
    x86::pic_remap();
    x86::idt_init();
    PCI::init();
    Disk::init();
    Multitask::init();

    x86::irq_register_handler(0, Timer::timer_callback);
    x86::pic_unmask_irq(0);

    x86::irq_register_handler(1, Keyboard::keyboard_callback);
    x86::pic_unmask_irq(1);

    x86::exception_register_handler(0x00, x86::Exceptions::division_error);
    x86::exception_register_handler(0x06, x86::Exceptions::invalid_opcode);
    x86::exception_register_handler(0x08, x86::Exceptions::double_fault);
    x86::exception_register_handler(0x0D, x86::Exceptions::general_protection_fault);

    x86::syscall_register_handler(SYS_EXIT, sys_exit);
    x86::syscall_register_handler(SYS_SLEEP, sys_sleep);
    x86::syscall_register_handler(SYS_PRINT, sys_print);

    kinfo(fmt("Kernel time: {} nanoseconds", Timer::ktime()));
    kinfo(fmt("Zero uptime: {} nanoseconds", Timer::uptime_ns()));

    // Initialize kernel stack
    static uint8_t initial_kstack[4096];
    x86::tss_entry.esp0 = (uint32_t)initial_kstack + 4096;
    x86::tss_entry.ss0 = 0x10;

    INT_ENABLE;
    SHOW_INT_ENABLE;

    VFS::init();

    RamFS::mount();

    auto ram = VFS::find_node("/ram");
    auto test = ram->create_dir(ram, "test");
    auto file = test->create_file(test, "file");
    auto text = "Hello, World!";
    file->write(file, 0, strlen(text), (uint8_t*)text);

    auto path = "/ram/test/file";
    auto found = VFS::find_node(path);
    auto buffer = new char[32];
    auto size = found->read(found, 0, 32, (uint8_t*)buffer);
    auto str = string(buffer, size);
    kinfo(fmt("Read '{}': {}", path, str));

    kwarn("You have reached the end of kernel control.");
    for (;;)
        CPU_HALT;
}
