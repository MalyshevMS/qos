#pragma once

#include <cstdint>

namespace Driver {
namespace Disk {
    void init();
    int device_count();
    bool get_device_info(int index, uint8_t& port, uint32_t& signature);
    bool read_sectors(int device_index, uint8_t* target, uint32_t LBA, uint8_t sector_count);
}; // namespace Disk
}; // namespace Driver