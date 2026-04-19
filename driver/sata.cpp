#include <driver/sata.hpp>
#include <driver/pci.hpp>
#include <kernel/ports.hpp>
#include <kernel/vconsole.hpp>
#include <klib/fmt.hpp>
#include <cfg/flags.txx>
#include <cstddef>
#include <cstdint>

namespace Driver::SATA {

using namespace Kernel;
using namespace Ports;
using namespace kstd;

constexpr uint8_t FIS_TYPE_REG_H2D = 0x27;
constexpr uint8_t ATA_CMD_READ_DMA_EXT = 0x25;
constexpr uint32_t HBA_PxCMD_ST = 1 << 0;
constexpr uint32_t HBA_PxCMD_FRE = 1 << 4;
constexpr uint32_t HBA_PxCMD_FR = 1 << 14;
constexpr uint32_t HBA_PxCMD_CR = 1 << 15;

struct HBA_PORT {
    uint32_t clb;
    uint32_t clbu;
    uint32_t fb;
    uint32_t fbu;
    uint32_t is;
    uint32_t ie;
    uint32_t cmd;
    uint32_t reserved0;
    uint32_t tfd;
    uint32_t sig;
    uint32_t ssts;
    uint32_t sctl;
    uint32_t serr;
    uint32_t sact;
    uint32_t ci;
    uint32_t sntf;
    uint32_t fbs;
    uint32_t reserved1[11];
    uint32_t vendor[4];
};

struct HBA_MEM {
    uint32_t cap;
    uint32_t ghc;
    uint32_t is;
    uint32_t pi;
    uint32_t vs;
    uint32_t ccc_ctl;
    uint32_t ccc_pts;
    uint32_t em_loc;
    uint32_t em_ctl;
    uint32_t cap2;
    uint32_t bohc;
    uint8_t reserved[0x100 - 0x2C];
    HBA_PORT ports[32];
};

struct HBA_CMD_HEADER {
    uint8_t cfl:5;
    uint8_t a:1;
    uint8_t w:1;
    uint8_t p:1;
    uint8_t r:1;
    uint8_t b:1;
    uint8_t c:1;
    uint8_t reserved0:1;
    uint16_t prdtl;
    uint32_t prdbc;
    uint32_t ctba;
    uint32_t ctbau;
    uint32_t reserved1[4];
} PACK;

struct HBA_PRDT_ENTRY {
    uint32_t dba;
    uint32_t dbau;
    uint32_t reserved0;
    uint32_t dbc;
} PACK;

struct HBA_CMD_TBL {
    uint8_t cfis[64];
    uint8_t acmd[16];
    uint8_t reserved[48];
    HBA_PRDT_ENTRY prdt_entry[1];
} PACK;

static volatile HBA_MEM* abar = nullptr;
static volatile HBA_PORT* sata_port = nullptr;
static bool sata_initialized = false;
static int active_port = -1;

alignas(1024) static uint8_t command_list[1024];
alignas(256) static uint8_t fis_receive[256];
alignas(128) static uint8_t command_table[4096];

static bool port_ready(volatile HBA_PORT* port) {
    uint32_t ssts = port->ssts;
    uint8_t det = ssts & 0x0F;
    uint8_t ipm = (ssts >> 8) & 0x0F;
    return det == 3 && ipm == 1;
}

static void start_cmd(volatile HBA_PORT* port) {
    while (port->cmd & HBA_PxCMD_CR) {}
    port->cmd |= HBA_PxCMD_FRE;
    port->cmd |= HBA_PxCMD_ST;
}

static void stop_cmd(volatile HBA_PORT* port) {
    port->cmd &= ~HBA_PxCMD_ST;
    port->cmd &= ~HBA_PxCMD_FRE;
    while (port->cmd & HBA_PxCMD_CR) {}
    while (port->cmd & HBA_PxCMD_FR) {}
}

static int find_command_slot(volatile HBA_PORT* port) {
    uint32_t slots = port->sact | port->ci;
    for (int i = 0; i < 32; i++) {
        if (!(slots & (1u << i))) {
            return i;
        }
    }
    return -1;
}

static void clear_memory(void* ptr, size_t size) {
    auto data = reinterpret_cast<uint8_t*>(ptr);
    for (size_t i = 0; i < size; i++) {
        data[i] = 0;
    }
}

bool init() {
    auto device = PCI::find_device(0x01, 0x06, 0x01);
    if (!device) {
        kinfo("SATA: AHCI controller not found.");
        return false;
    }

    uintptr_t base = (uintptr_t)(device->bar[5] & ~0xF);
    if (!base) {
        kwarn("SATA: AHCI base address is invalid.");
        return false;
    }

    abar = reinterpret_cast<volatile HBA_MEM*>(base);
    abar->ghc |= 1u << 31; // AE
    abar->ghc |= 1u << 1;  // IE

    uint32_t ports_impl = abar->pi;
    for (int i = 0; i < 32; i++) {
        if (!(ports_impl & (1u << i))) {
            continue;
        }

        volatile HBA_PORT* port = &abar->ports[i];
        if (!port_ready(port)) {
            continue;
        }

        uint32_t sig = port->sig;
        if (sig == 0x00000101 || sig == 0xEB140101) {
            active_port = i;
            sata_port = port;
            break;
        }
    }

    if (active_port < 0 || !sata_port) {
        kwarn("SATA: no active SATA device found on AHCI ports.");
        return false;
    }

    clear_memory(command_list, sizeof(command_list));
    clear_memory(fis_receive, sizeof(fis_receive));
    clear_memory(command_table, sizeof(command_table));

    stop_cmd(sata_port);

    sata_port->clb = (uint32_t)(uintptr_t)command_list;
    sata_port->clbu = 0;
    sata_port->fb = (uint32_t)(uintptr_t)fis_receive;
    sata_port->fbu = 0;
    sata_port->is = 0xFFFFFFFFu;
    sata_port->ie = 0;

    start_cmd(sata_port);

    sata_initialized = true;
    kinfo(fmt("SATA: AHCI initialized on port {}.", active_port));
    return true;
}

bool read_sectors(uint8_t* target, uint32_t LBA, uint8_t sector_count) {
    if (!sata_initialized || !sata_port || sector_count == 0) {
        return false;
    }

    if (sector_count > 0xFFu) {
        return false;
    }

    while (sata_port->tfd & 0x80) {}

    int slot = find_command_slot(sata_port);
    if (slot < 0) {
        kwarn("SATA: no command slot available.");
        return false;
    }

    auto cmd_header = reinterpret_cast<HBA_CMD_HEADER*>(command_list) + slot;
    clear_memory(cmd_header, sizeof(HBA_CMD_HEADER));
    cmd_header->cfl = 5;
    cmd_header->w = 0;
    cmd_header->prdtl = 1;
    cmd_header->ctba = (uint32_t)(uintptr_t)command_table;
    cmd_header->ctbau = 0;

    auto cmd_table = reinterpret_cast<HBA_CMD_TBL*>(command_table);
    clear_memory(cmd_table, sizeof(HBA_CMD_TBL));

    cmd_table->prdt_entry[0].dba = (uint32_t)(uintptr_t)target;
    cmd_table->prdt_entry[0].dbau = 0;
    cmd_table->prdt_entry[0].dbc = (uint32_t)(512u * sector_count - 1);
    cmd_table->prdt_entry[0].dbc |= 1u << 31;

    auto cfis = cmd_table->cfis;
    cfis[0] = FIS_TYPE_REG_H2D;
    cfis[1] = 1 << 7;
    cfis[2] = ATA_CMD_READ_DMA_EXT;
    cfis[3] = 0;
    cfis[4] = (uint8_t)LBA;
    cfis[5] = (uint8_t)(LBA >> 8);
    cfis[6] = (uint8_t)(LBA >> 16);
    cfis[7] = 1 << 6;
    cfis[8] = (uint8_t)(LBA >> 24);
    cfis[9] = 0;
    cfis[10] = 0;
    cfis[11] = 0;
    cfis[12] = sector_count;
    cfis[13] = 0;

    sata_port->is = 0xFFFFFFFFu;
    sata_port->ci = (1u << slot);

    int timeout = 0;
    while (sata_port->ci & (1u << slot)) {
        if (sata_port->tfd & 0x01) {
            kwarn("SATA: command failed during transfer.");
            break;
        }
        if (++timeout > 1'000'000) {
            kwarn("SATA: command timeout.");
            return false;
        }
    }

    while (sata_port->tfd & 0x80) {}
    if (sata_port->tfd & 0x01) {
        kwarn("SATA: device reported error after transfer.");
        return false;
    }

    return true;
}

} // namespace Driver::SATA
