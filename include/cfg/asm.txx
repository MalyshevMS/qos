#define INT_ENABLE          asm volatile("sti")
#define INT_DISABLE         asm volatile("cli")
#define CPU_HALT            asm volatile("hlt")
#define CPU_PAUSE           asm volatile("pause") // Don't do SHOW_CPU_PAUSE!

#define SHOW_INT_ENABLE     kinfo("CPU: INTERRUPTS ENABLED")
#define SHOW_INT_DISABLE    kinfo("CPU: INTERRUPTS DISABLED")
#define SHOW_CPU_HALT       kinfo("CPU: HALTING")

#define EOI_MASTER outb(0x20, 0x20)
#define EOI_SLAVE  outb(0xA0, 0x20)