#include <driver/disk.hpp>
#include <driver/sata.hpp>
#include <kernel/ports.hpp>
#include <kernel/serial.hpp>

using namespace Kernel;
using namespace Ports;

namespace Driver::Disk {

static bool sata_initialized = false;

void init() {
    sata_initialized = SATA::init();
}

int device_count() {
    return SATA::get_device_count();
}

bool get_device_info(int index, uint8_t& port, uint32_t& signature) {
    return SATA::get_device_info(index, port, signature);
}

bool read_sectors(int device_index, uint8_t* target, uint32_t LBA, uint8_t sector_count) {
    if (!sata_initialized) {
        return false;
    }

    return SATA::read_sectors(device_index, target, LBA, sector_count);
}

} // namespace Driver::Disk