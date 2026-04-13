#include <kernel/power.hpp>
#include <kernel/ports.hpp>
#include <kernel/serial.hpp>
#include <cfg/asm.txx>
#include <cstdint>

using namespace Kernel;
using namespace Ports;

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

inline uint16_t get_s5_type(FADT* fadt) {
    unsigned char* dsdt = (unsigned char*)fadt->Dsdt;
    uint32_t length = ((ACPISDTHeader*)dsdt)->Length;
    
    dsdt += sizeof(ACPISDTHeader);
    length -= sizeof(ACPISDTHeader);

    for (uint32_t i = 0; i < length - 4; i++) {
        if (memcmp(&dsdt[i], "_S5_", 4) == 0) {
            i += 4;
            
            if (dsdt[i] == 0x12) i++;
            
            i++; 
            if (dsdt[i] == 0x0A) i++;
            
            i++;
            
            if (dsdt[i] == 0x0A) i++; // Это пиздец бля. Я хуже функции в жизни не видел.
            
            return dsdt[i];
        }
    }
    return 0x5;
}

namespace Kernel {
namespace Hardware {

static bool acpi_poweroff() {
    FADT* fadt = find_fadt();
    
    // Validate critical parameters
    if (!fadt) {
        Serial::println("FADT not found!");
        return false;
    }

    if (fadt->PM1a_CNT_BLK == 0) {
        Serial::println("PM1a_CNT_BLK is 0!");
        return false;
    }

    Serial::println("Checking ACPI state...");
    uint16_t pm1a_status = inw(fadt->PM1a_CNT_BLK);
    if ((pm1a_status & 1) == 0) {
        Serial::println("ACPI is disabled, enabling...");
        if (fadt->SMI_CMD != 0 && fadt->ACPI_ENABLE != 0) {
            outb(fadt->SMI_CMD, fadt->ACPI_ENABLE);
            
            for (int timeout = 0; timeout < 3000; timeout++) {
                pm1a_status = inw(fadt->PM1a_CNT_BLK);
                if ((pm1a_status & 1) == 1) {
                    Serial::println("ACPI enabled successfully!");
                    break;
                }
            }
            
            if ((pm1a_status & 1) == 0) {
                Serial::println("ACPI enable timeout, continuing anyway...");
            }
        }
    } else {
        Serial::println("ACPI is already enabled!");
    }

    uint16_t SLP_TYP = get_s5_type(fadt);
    uint16_t SLP_EN = 1 << 13;

    
    uint16_t val = inw(fadt->PM1a_CNT_BLK);
    val &= ~(7 << 10);  // Clear SLP_TYP field
    val |= (SLP_TYP << 10) | SLP_EN;
    
    outw(fadt->PM1a_CNT_BLK, val);
    
    if (fadt->PM1b_CNT_BLK != 0) {
        outw(fadt->PM1b_CNT_BLK, (SLP_TYP << 10) | SLP_EN);
    }

    return true;
}

void poweroff() {
    // Try ACPI first, don't disable interrupts yet
    if (acpi_poweroff()) {
        INT_DISABLE;
        
        volatile unsigned int count = 0;
        while (count < 50000000) count++;
        
        Serial::println("Power off failed. Hanging...");
        for (;;) CPU_HALT;
    } else {
        Serial::println("ACPI poweroff failed! Falling back to indefinite halt.");
        Serial::println("System will remain powered but halted.");
        INT_DISABLE;
    }

    Serial::println("Power off failed. Hanging...");
    for (;;) CPU_HALT;
}

void reboot() {
    
    uint8_t val = 0x02;
    int timeout = 0;
    int max_timeout = 1000000;
    
    // Wait for keyboard controller to be ready (bit 1 clear)
    for (timeout = 0; timeout < max_timeout && (val & 0x02); timeout++) {
        val = inb(0x64);
    }

    outb(0x64, 0xFE);
    
    INT_DISABLE;
    
    volatile unsigned int reboot_wait = 0;
    while (reboot_wait < 500000000) reboot_wait++;
    
    Serial::println("Reboot failed. Hanging...");
    for (;;) CPU_HALT;
}

}; // namespace Hardware
}; // namespace Kernel