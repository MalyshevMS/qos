#include <driver/usb.hpp>
#include <kernel/vconsole.hpp>
#include <klib/fmt.hpp>
#include <arch/x86/idt.hpp>
#include <arch/x86/pic.hpp>

namespace Driver::USB {

    using namespace Arch;
    using namespace Kernel;
    using namespace kstd;

    static USBController controllers[8];
    static int controller_count = 0;

    struct EHCI_Regs {
        volatile uint32_t usbcmd;
        volatile uint32_t usbsts;
        volatile uint32_t usbintr;
        volatile uint32_t frindex;
        volatile uint32_t ctrldssegment;
        volatile uint32_t periodiclistbase;
        volatile uint32_t asynclistaddr;
    };

    static void usb_ehci_handler(x86::Registers* regs) {
        uint32_t op_regs = 0xfebf2020 + 0x20; 
        uint32_t* usbsts = (uint32_t*)(op_regs + 0x04);

        uint32_t status = *usbsts;

        if (status & 0x3F) {
            kinfo(fmt("USB: IRQ caught! Status: %x", status));
            
            *usbsts = status; 
        }

        pic_send_eoi(regs->int_no);
}

    void init_ehci(PCI::PCIDevice* dev) {
        uint32_t bar = dev->bar[0] & 0xFFFFFFF0;
        
        uint8_t cap_length = *((volatile uint8_t*)bar);
        EHCI_Regs* regs = (EHCI_Regs*)(bar + cap_length);

        kinfo(fmt("USB: Initializing EHCI at MMIO %x", (uint32_t)regs));

        regs->usbcmd |= (1 << 1); 
        while (regs->usbcmd & (1 << 1));

        regs->usbintr = 0x3F; 

        regs->usbcmd |= 1;

        volatile uint32_t* config_flag = (volatile uint32_t*)((uint32_t)regs + 0x40);
        *config_flag = 1;

        uint8_t vector = 0x20 + dev->interrupt_line;
        
        kinfo(fmt("USB: setting up interrupt line for IRQ %x", vector));
        
        x86::irq_register_handler(vector, (x86::handler_t)&usb_ehci_handler);
        x86::pic_unmask_irq(dev->interrupt_line);
    }

    void init() {
        controller_count = 0;
        auto devices = PCI::get_devices();
        
        for (int i = 0; i < PCI::get_device_count(); i++) {
            auto& dev = const_cast<PCI::PCIDevice&>(devices[i]);
            
            if (dev.class_code == 0x0C && dev.subclass == 0x03) {
                if (dev.prog_if == 0x20) { // EHCI
                    controllers[controller_count].pci_dev = &dev;
                    controllers[controller_count].type = USB_EHCI;
                    init_ehci(&dev);
                    controller_count++;
                }
            }
        }
    }

} // namespace Driver::USB