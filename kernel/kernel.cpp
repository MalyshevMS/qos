#include <cfg/asm.txx>
#include <cfg/flags.txx>

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

using namespace Kernel;
using namespace Mem;
using namespace Arch;
using namespace Driver;
using namespace FS;
using namespace Syscall;
using namespace kstd;

uint32_t version_read(VFSNode* node, uint32_t offset, uint32_t size, uint8_t* buff) {
    const char *data = "QOS v1.0\n";
    uint32_t len = strlen(data);

    if (offset >= len)
        return 0; // End of file
    if (offset + size > len)
        size = len - offset; // Truncate

    memcpy(buff, (uint8_t *)data + offset, size);

    return size;
}

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

    vfs_init();

    auto procfs = new VFSNode;
    memset(procfs, 0, sizeof(VFSNode));
    memcpy(procfs->name, "proc", 5);
    procfs->type = FS_DIR;
    procfs->finddir = vfs_finddir;

    auto version = new VFSNode;
    memset(version, 0, sizeof(VFSNode));
    memcpy(version, "version", 8);
    version->type = FS_FILE;
    version->read = version_read;
    version->size = 10;

    vfs_root->map["proc"] = procfs;
    procfs->map["version"] = version;

    auto fproc = vfs_root->finddir(vfs_root, "proc");
    auto fver = fproc->finddir(fproc, "version");

    auto buf = new char[16];
    auto sz = fver->read(fver, 0, 16, (uint8_t*)buf);
    auto str = string(buf, sz);

    kinfo(fmt("/proc/version.read: '{}'", str));

    // Shouldn't be reached
    kwarn("You have reached the end of kernel control.");
    for (;;)
        CPU_HALT;
}
