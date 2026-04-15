#pragma once

#include <cstdint>

namespace Driver {
namespace ACPI {
    struct RSDPDescriptor {
        char Signature[8];
        uint8_t Checksum;
        char OEMID[6];
        uint8_t Revision;
        uint32_t RsdtAddress;
    } __attribute__((packed));

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
    } __attribute__((packed));

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
    } __attribute__((packed));

    void init();
    
    RSDPDescriptor* find_rsdp();
    ACPISDTHeader* find_table(const char* name);
    
    uint16_t get_s5_type();

    bool poweroff();
}; // namespace ACPI
}; // namespace Driver