#pragma once

#include <driver/pci.hpp>

namespace Driver::USB {

    struct USBController {
        PCI::PCIDevice* pci_dev;
        uint32_t mmio_base;
        uint8_t  irq;
        uint8_t  type;
    };

    enum ControllerType {
        USB_EHCI = 2,
        USB_XHCI = 3
    };

    void init();
    
    void handle_interrupt(uint8_t irq);

} // namespace Driver::USB