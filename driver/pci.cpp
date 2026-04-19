#include <driver/pci.hpp>
#include <kernel/ports.hpp>
#include <kernel/vconsole.hpp>
#include <klib/fmt.hpp>

namespace Driver::PCI {
        
    using namespace Kernel;
    using namespace Ports;
    using namespace kstd;

    static PCIDevice devices[32];
    static int device_count = 0;

    uint32_t read_dword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
        uint32_t address = (uint32_t)((uint32_t)bus << 16) | 
                        ((uint32_t)slot << 11) |
                        ((uint32_t)func << 8) | 
                        (offset & 0xFC) | 
                        ((uint32_t)0x80000000);
        outl(CONFIG_ADDRESS, address);
        return inl(CONFIG_DATA);
    }

    void write_dword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t data) {
        uint32_t address = (uint32_t)((uint32_t)bus << 16) | 
                        ((uint32_t)slot << 11) |
                        ((uint32_t)func << 8) | 
                        (offset & 0xFC) | 
                        ((uint32_t)0x80000000);
        outl(CONFIG_ADDRESS, address);
        outl(CONFIG_DATA, data);
    }

    void check_device(uint8_t bus, uint8_t device, uint8_t function) {
        uint32_t reg0 = read_dword(bus, device, function, 0);
        uint16_t vendor_id = reg0 & 0xFFFF;
        
        if (vendor_id == 0xFFFF) return; // Устройства нет

        PCIDevice& dev = devices[device_count];
        dev.bus = bus;
        dev.device = device;
        dev.function = function;
        dev.vendor_id = vendor_id;
        dev.device_id = (reg0 >> 16) & 0xFFFF;

        // Читаем класс и подкласс (регистр 0x08)
        uint32_t reg8 = read_dword(bus, device, function, 0x08);
        dev.class_code = (reg8 >> 24) & 0xFF;
        dev.subclass   = (reg8 >> 16) & 0xFF;
        dev.prog_if    = (reg8 >> 8) & 0xFF;

        // Читаем BAR'ы (регистры 0x10 - 0x24)
        for (int i = 0; i < 6; i++) {
            dev.bar[i] = read_dword(bus, device, function, 0x10 + (i * 4));
        }

        kinfo(fmt("PCI: [%x:%x.%x] ID:%x:%x Class:%x Sub:%x", bus, device, function, dev.vendor_id, dev.device_id, dev.class_code, dev.subclass));

        device_count++;
    }

    void init() {
        kinfo("PCI: Enumerating devices...");
        device_count = 0;

        // Перебор всех шин, устройств и функций
        for (int bus = 0; bus < 256; bus++) {
            for (int dev = 0; dev < 32; dev++) {
                // Проверяем 0-ю функцию, если она мультифункциональная — проверим остальные
                uint32_t header_type = (read_dword(bus, dev, 0, 0x0C) >> 16) & 0xFF;
                
                check_device(bus, dev, 0);
                
                if (header_type & 0x80) { // Мультифункциональное устройство
                    for (int func = 1; func < 8; func++) {
                        check_device(bus, dev, func);
                    }
                }
            }
        }
    }

    PCIDevice* find_device(uint8_t class_code, uint8_t subclass) {
        for (int i = 0; i < device_count; i++) {
            if (devices[i].class_code == class_code && devices[i].subclass == subclass) {
                return &devices[i];
            }
        }
        return nullptr;
    }

    PCIDevice* find_device(uint8_t class_code, uint8_t subclass, uint8_t prog_if) {
        for (int i = 0; i < device_count; i++) {
            if (devices[i].class_code == class_code && devices[i].subclass == subclass) {
                if (prog_if == 0xFF || devices[i].prog_if == prog_if) {
                    return &devices[i];
                }
            }
        }
        return nullptr;
    }

} // namespace Driver::PCI