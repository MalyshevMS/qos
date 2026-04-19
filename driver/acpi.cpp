#include <driver/acpi.hpp>
#include <klib/mem.hpp>
#include <klib/cstring.hpp>
#include <kernel/serial.hpp>
#include <kernel/ports.hpp>

using namespace kstd;
using namespace Kernel;
using namespace Ports;

namespace Driver::ACPI {

RSDPDescriptor* rsdp = nullptr;
FADT* fadt = nullptr;

void init() {
    rsdp = find_rsdp();
    fadt = (FADT*)find_table("FACP");
    
    if (!fadt) {
        kwarn("FADT not found!");
        return;
    }

    if (fadt->PM1a_CNT_BLK == 0) {
        kwarn("PM1a_CNT_BLK is 0!");
        return;
    }
}

RSDPDescriptor* find_rsdp() {
    uint8_t* p = (uint8_t*)0x000E0000;
    RSDPDescriptor* rsdp = nullptr;

    for (; (uintptr_t)p < 0x000FFFFF; p += 16) {
        if (memcmp(p, "RSD PTR ", 8) == 0) {
            rsdp = (RSDPDescriptor*)p;
            break;
        }
    }

    if (!rsdp) return nullptr;

    return rsdp;
};

ACPISDTHeader* find_table(const char* name) {
    ACPISDTHeader* rsdt = (ACPISDTHeader*)(uintptr_t)rsdp->RsdtAddress;
    uint32_t entries = (rsdt->Length - sizeof(ACPISDTHeader)) / 4;
    uint32_t* second_headers = (uint32_t*)((uintptr_t)rsdt + sizeof(ACPISDTHeader));

    for (uint32_t i = 0; i < entries; i++) {
        ACPISDTHeader* h = (ACPISDTHeader*)(uintptr_t)second_headers[i];
        if (memcmp(h->Signature, name, strlen(name)) == 0) {
            return h;
        }
    }
    return nullptr;
}

uint16_t get_s5_type() {
    if (!fadt) {
        kwarn("FADT not initilized. Using Default S5 type: 0x5");
        return 0x5;
    }

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

bool poweroff() {
    // Validate critical parameters
    if (!fadt) {
        kwarn("FADT not found!");
        return false;
    }

    if (fadt->PM1a_CNT_BLK == 0) {
        kwarn("PM1a_CNT_BLK is 0!");
        return false;
    }

    uint16_t pm1a_status = inw(fadt->PM1a_CNT_BLK);
    if ((pm1a_status & 1) == 0 && fadt->SMI_CMD != 0 && fadt->ACPI_ENABLE != 0) {
        outb(fadt->SMI_CMD, fadt->ACPI_ENABLE);
        
        for (int timeout = 0; timeout < 3000; timeout++) {
            pm1a_status = inw(fadt->PM1a_CNT_BLK);
            if ((pm1a_status & 1) == 1) break;
        }
    }

    uint16_t SLP_TYP = get_s5_type();
    uint16_t SLP_EN = 1 << 13;

    
    uint16_t val = inw(fadt->PM1a_CNT_BLK);
    val &= ~(7 << 10);  // Clear SLP_TYP field
    val |= (SLP_TYP << 10) | SLP_EN;
    
    kinfo("Powered off.");
    outw(fadt->PM1a_CNT_BLK, val);
    
    if (fadt->PM1b_CNT_BLK != 0) {
        outw(fadt->PM1b_CNT_BLK, (SLP_TYP << 10) | SLP_EN);
    }

    return true;
}

uint64_t get_hpet_address() {
    HPET* hpet = (HPET*)find_table("HPET");
    if (hpet) return hpet->address;
    return 0;
}

}; // namespace Driver::ACPI