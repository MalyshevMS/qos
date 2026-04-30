#define SYSCALLS_COUNT 32

#define SYSCALL(f) uint32_t sys_##f(Arch::x86::Registers* regs)

#define SYS_NORETURN return (uint32_t)regs

#define SYS_EXIT            1