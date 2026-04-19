#include <driver/disk.hpp>
#include <driver/sata.hpp>
#include <kernel/ports.hpp>
#include <kernel/serial.hpp>

using namespace Kernel;
using namespace Ports;

namespace Driver::Disk {

static bool sata_available = false;

void init() {
    sata_available = SATA::init();
}

bool read_sectors(uint8_t* target, uint32_t LBA, uint8_t sector_count) {
    if (sata_available) {
        if (SATA::read_sectors(target, LBA, sector_count)) {
            return true;
        }
    }

    read_sectors_ATA_PIO(target, LBA, sector_count);
    return true;
}

void read_sectors_ATA_PIO(uint8_t* target, uint32_t LBA, uint8_t sector_count) {
    outb(0x1F6, (LBA >> 24) | 0xE0);
    outb(0x1F2, sector_count);
    outb(0x1F3, (uint8_t)LBA);
    outb(0x1F4, (uint8_t)(LBA >> 8));
    outb(0x1F5, (uint8_t)(LBA >> 16));
    
    outb(0x1F7, 0x20);

    int disk_wait_counter = 0;
    while (!(inb(0x1F7) & 0x08)) {
        kinfo("Waiting for disk ready...");
        disk_wait_counter++;
        if (disk_wait_counter > 10) {
            kwarn("Disk not found.");
            for (int i = 0; i < 512 * sector_count; i++) {
                target[i] = 0;
            }
            return;
        }
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

} // namespace Driver::Disk