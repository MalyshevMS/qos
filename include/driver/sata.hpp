#pragma once

#include <cstdint>

namespace Driver {
namespace SATA {
    bool init();
    int get_device_count();
    bool get_device_info(int index, uint8_t& port, uint32_t& signature);
    bool read_sectors(int device_index, uint8_t* target, uint32_t LBA, uint8_t sector_count);
}; // namespace SATA
}; // namespace Driver
