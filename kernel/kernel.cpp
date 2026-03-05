#include <stdint.h>

#define VGA ((volatile uint16_t*)0xB8000)
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

void cls() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        VGA[i] = 0x001f;
    }

    VGA[3] = 0x1f41;
}

extern "C" void kernel_main() {
    cls();
    
    VGA[0] = 0x1f41;
    VGA[1] = 0x1f41;
    // VGA[2] = 0x1f41;
    
    while (1) {
        asm volatile ("hlt");
    }
}