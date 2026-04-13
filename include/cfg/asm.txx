#define INT_ENABLE          asm volatile("sti")
#define INT_DISABLE         asm volatile("cli")
#define CPU_HALT            asm volatile("hlt")

#define SHOW_INT_ENABLE     Serial::println("CPU:\tINTERRUPTS ENABLED")
#define SHOW_INT_DISABLE    Serial::println("CPU:\tINTERRUPTS DISABLED")
#define SHOW_CPU_HALT       Serial::println("CPU:\tHALTING")
