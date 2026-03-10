#pragma once

namespace Arch::x86 {
    void pic_remap();
    void pic_unmask_irq(int irq);
}