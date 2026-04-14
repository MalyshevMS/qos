#include <driver/disk.hpp>
#include <kernel/ports.hpp>
#include <kernel/serial.hpp>

using namespace Driver;
using namespace Kernel;
using namespace Ports;

void Disk::read_sectors_ATA_PIO(uint8_t* target, uint32_t LBA, uint8_t sector_count){
    outb(0x1F6, (LBA >> 24) | 0xE0);
    outb(0x1F2, sector_count);
    outb(0x1F3, (uint8_t)LBA);
    outb(0x1F4, (uint8_t)(LBA >> 8));
    outb(0x1F5, (uint8_t)(LBA >> 16));
    
    outb(0x1F7, 0x20);

    while (!(inb(0x1F7) & 0x08)) {
        Serial::println("Waiting for disk ready...");
    }

    for (int j = 0; j < sector_count; j++) {
        for (int i = 0; i < 256; i++) {
            auto data = inw(0x1F0);
            target[i * 2] = data & 0xFF;
            target[i * 2 + 1] = data >> 8;
        }
        target += 256;
    }
}