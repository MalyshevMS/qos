#pragma once

#include <cstdint>
#include <cfg/flags.txx>

namespace Driver {
namespace ACPI {
    struct RSDPDescriptor {
        char Signature[8];
        uint8_t Checksum;
        char OEMID[6];
        uint8_t Revision;
        uint32_t RsdtAddress;
    } PACK;

    struct ACPISDTHeader {
        char Signature[4];
        uint32_t Length;
        uint8_t Revision;
        uint8_t Checksum;
        char OEMID[6];
        char OEMTableID[8];
        uint32_t OEMRevision;
        uint32_t CreatorID;
        uint32_t CreatorRevision;
    } PACK;

    struct FADT {
        struct ACPISDTHeader h;
        uint32_t FirmwareCtrl;
        uint32_t Dsdt;
        uint8_t  Reserved;
        uint8_t  PreferredPowerManagementProfile;
        uint16_t SCI_Interrupt;
        uint32_t SMI_CMD;
        uint8_t  ACPI_ENABLE;
        uint8_t  ACPI_DISABLE;
        uint8_t  S4BIOS_REQ;
        uint8_t  PSTATE_CNT;
        uint32_t PM1a_EVT_BLK;
        uint32_t PM1b_EVT_BLK;
        uint32_t PM1a_CNT_BLK;
        uint32_t PM1b_CNT_BLK;
    } PACK;

    struct HPET {
        struct ACPISDTHeader h;
        uint8_t hardware_rev_id;
        uint8_t comparator_count : 5;
        uint8_t counter_size : 1;
        uint8_t reserved : 1;
        uint8_t legacy_replacement : 1;
        uint16_t pci_vendor_id;
        uint8_t address_space_id;    // 0 = Memory, 1 = I/O
        uint8_t register_bit_width;
        uint8_t register_bit_offset;
        uint8_t reserved2;
        uint64_t address;            // Базовый адрес HPET
        uint8_t hpet_number;
        uint16_t minimum_tick;
        uint8_t page_protection;
    } PACK;

    void init();
    
    RSDPDescriptor* find_rsdp();
    ACPISDTHeader* find_table(const char* name);
    
    uint16_t get_s5_type();
    uint64_t get_hpet_address();

    bool poweroff();
}; // namespace ACPI
}; // namespace Driver