#define SYSCALLS_COUNT 32

#define SYSCALL_ARGS Arch::x86::Registers* regs, uint32_t& result_esp

#define SYS_EXIT            1
#define SYS_SLEEP           2
#define SYS_PRINT           3