#pragma once

namespace Arch::x86 {
    void pic_remap();
    void pic_unmask_irq(int irq);
}

extern "C" void pic_send_eoi(int irq);