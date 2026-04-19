#pragma once

#include <cstdint>

namespace Driver {
namespace SATA {
    bool init();
    bool read_sectors(uint8_t* target, uint32_t LBA, uint8_t sector_count);
}; // namespace SATA
}; // namespace Driver
