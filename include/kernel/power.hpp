#pragma once

#include <cstdint>
#include <klib/mem.hpp>
#include <kernel/ports.hpp>

using namespace Kernel::Ports;
using namespace kstd;

namespace Kernel {
namespace Hardware {
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

    inline bool check_sum(struct ACPISDTHeader* table) {
        unsigned char sum = 0;
        for (uint32_t i = 0; i < table->Length; i++) {
            sum += ((char*)table)[i];
        }
        return sum == 0;
    }

    inline FADT* find_fadt() {
        uint8_t* p = (uint8_t*)0x000E0000;
        RSDPDescriptor* rsdp = nullptr;

        for (; (uintptr_t)p < 0x000FFFFF; p += 16) {
            if (memcmp(p, "RSD PTR ", 8) == 0) {
                rsdp = (RSDPDescriptor*)p;
                break;
            }
        }

        if (!rsdp) return nullptr;

        ACPISDTHeader* rsdt = (ACPISDTHeader*)(uintptr_t)rsdp->RsdtAddress;
        uint32_t entries = (rsdt->Length - sizeof(ACPISDTHeader)) / 4;
        uint32_t* second_headers = (uint32_t*)((uintptr_t)rsdt + sizeof(ACPISDTHeader));

        for (uint32_t i = 0; i < entries; i++) {
            ACPISDTHeader* h = (ACPISDTHeader*)(uintptr_t)second_headers[i];
            if (memcmp(h->Signature, "FACP", 4) == 0) {
                return (FADT*)h;
            }
        }
        return nullptr;
    }

    inline void acpi_shutdown() {
        FADT* fadt = find_fadt();
        if (!fadt) return;

        if ((inw(fadt->PM1a_CNT_BLK) & 1) == 0) {
            if (fadt->SMI_CMD != 0 && fadt->ACPI_ENABLE != 0) {
                outb(fadt->SMI_CMD, fadt->ACPI_ENABLE);
                int timeout = 0;
                while (timeout < 300) {
                    if ((inw(fadt->PM1a_CNT_BLK) & 1) == 1) break;
                    timeout++;
                }
            }
        }

        uint16_t SLP_TYP = 0x5; // TODO:  obtain from DSDT
        uint16_t SLP_EN = 1 << 13;

        outw(fadt->PM1a_CNT_BLK, (SLP_TYP << 10) | SLP_EN);
        
        if (fadt->PM1b_CNT_BLK != 0) {
            outw(fadt->PM1b_CNT_BLK, (SLP_TYP << 10) | SLP_EN);
        }
    }

    void reboot();
    inline void poweroff() { acpi_shutdown(); };
}; // namespace Hardware
}; // namespace Kernel