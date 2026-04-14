#pragma once

#include <cstdint>

namespace Driver {
namespace Disk {
    void read_sectors_ATA_PIO(uint8_t* target, uint32_t LBA, uint8_t sector_count);
}; // namespace Disk
}; // namespace Driver