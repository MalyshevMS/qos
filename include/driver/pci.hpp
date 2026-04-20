#pragma once

#include <cstdint>

namespace Driver {
namespace PCI {
    struct PCIDevice {
        uint8_t  bus;
        uint8_t  device;
        uint8_t  function;
        uint16_t vendor_id;
        uint16_t device_id;
        uint8_t  class_code;
        uint8_t  subclass;
        uint8_t  prog_if;
        uint32_t bar[6];
        uint8_t  interrupt_line;
    };

    const uint16_t CONFIG_ADDRESS = 0xCF8;
    const uint16_t CONFIG_DATA    = 0xCFC;

    uint32_t read_dword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
    void write_dword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t data);

    void init();
    PCIDevice* find_device(uint8_t class_code, uint8_t subclass);
    PCIDevice* find_device(uint8_t class_code, uint8_t subclass, uint8_t prog_if);

    const PCIDevice* get_devices();
    int get_device_count();
}; // namespace PCI
}; // namespace Driver