#pragma once

#include <cstdint>

namespace Driver {
namespace Disk {
    void init();
    bool read_sectors(uint8_t* target, uint32_t LBA, uint8_t sector_count);
    void read_sectors_ATA_PIO(uint8_t* target, uint32_t LBA, uint8_t sector_count);
}; // namespace Disk
}; // namespace Driver