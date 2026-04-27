#pragma once

#include <cstdint>
#include <klib/string.hpp>

namespace kstd {
    int32_t to_int32(const string& str);
    uint32_t to_uint32(const string& str);
}; // namespace kstd