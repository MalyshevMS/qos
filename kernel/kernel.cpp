#include "klib/cstring.hpp"
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
    RamFS::create_dir(RamFS::ramfs_root, "dir");
    RamFS::create_file(static_cast<RamFS::RamNode*>(VFS::find_node("/ram/dir")), "file.txt");

    auto file = VFS::find_node("/ram/dir/file.txt");
    auto str = "Hello";
    file->write(file, 0, strlen(str), (uint8_t*)str);

    auto str2 = "World++!";
    file->write(file, 3, strlen(str2), (uint8_t*)str2);

    auto bf = new char[16];
    auto sz = file->read(file, 0, 16, (uint8_t*)bf);
    auto s = string(bf, sz);
    kinfo(fmt("Read '/ram/dir/file.txt': {}", s));

    kwarn("You have reached the end of kernel control.");
    for (;;)
        CPU_HALT;
}
