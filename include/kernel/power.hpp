#pragma once

#include <cstdint>
#include <klib/mem.hpp>
#include <kernel/ports.hpp>

using namespace Kernel::Ports;
using namespace kstd;

namespace Kernel {
namespace Hardware {
    void reboot();
    void poweroff();
}; // namespace Hardware
}; // namespace Kernel