#include <klib/conv.hpp>
#include <kernel/vconsole.hpp>

namespace kstd {

using namespace Kernel;

int32_t to_int32(const string& str) {
    int32_t result = 0;
    int i = 0;
    bool isNegative = false;

    while (i < str.size() && str[i] == ' ') {
        i++;
    }

    if (i < str.size() && (str[i] == '-' || str[i] == '+')) {
        if (str[i] == '-') {
            isNegative = true;
        }
        i++;
    }

    while (i < str.size() && str[i] >= '0' && str[i] <= '9') {
        int digit = str[i] - '0';

        if (result > (INT32_MAX - digit) / 10) {
            kwarn("Owerflow while converting from string to int32.");
            return isNegative ? INT32_MIN : INT32_MAX;
        }

        result = result * 10 + digit;
        i++;
    }

    return isNegative ? -result : result;
}

uint32_t to_uint32(const string& str) {
    uint32_t result = 0;
    size_t i = 0;

    while (i < str.size() && str[i] == ' ') {
        i++;
    }

    if (i < str.size() && str[i] == '+') {
        i++;
    }

    while (i < str.size() && str[i] >= '0' && str[i] <= '9') {
        uint32_t digit = str[i] - '0';

        if (result > (UINT32_MAX - digit) / 10) {
            kwarn("Owerflow while converting from string to uint32.");
            return UINT32_MAX;
        }

        result = result * 10 + digit;
        i++;
    }

    return result;
}

}; // namespace kstd