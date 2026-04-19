#include <cstdint>
#include <kernel/vconsole.hpp>

using namespace Kernel;

extern "C" {

typedef struct {
    uint64_t quot;
    uint64_t rem;
} uint64_div_t;

typedef struct {
    int64_t quot;
    int64_t rem;
} int64_div_t;

uint64_div_t uint64_divmod(uint64_t dividend, uint64_t divisor) {
    uint64_div_t result;
    if (divisor == 0) {
        kpanic("Division by zero!");
        result.quot = 0;
        result.rem = 0;
        return result;
    }

    uint64_t quotient = 0;
    uint64_t remainder = 0;

    for (int i = 63; i >= 0; i--) {
        remainder <<= 1;
        remainder |= (dividend >> i) & 1;

        if (remainder >= divisor) {
            remainder -= divisor;
            quotient |= (1ULL << i);
        }
    }

    result.quot = quotient;
    result.rem = remainder;
    return result;
}

int64_div_t int64_divmod(int64_t dividend, int64_t divisor) {
    int64_div_t result;
    if (divisor == 0) {
        kpanic("Division by zero!");
        result.quot = 0;
        result.rem = 0;
        return result;
    }

    int64_t quotient = 0;
    int64_t remainder = 0;

    for (int i = 63; i >= 0; i--) {
        remainder <<= 1;
        remainder |= (dividend >> i) & 1;

        if (remainder >= divisor) {
            remainder -= divisor;
            quotient |= (1ULL << i);
        }
    }

    result.quot = quotient;
    result.rem = remainder;
    return result;
}

// GCC division (/)
uint64_t __udivdi3(uint64_t a, uint64_t b) {
    return uint64_divmod(a, b).quot;
}

int64_t __divdi3(int64_t a, int64_t b) {
    return uint64_divmod(a, b).quot;
}

// GCC mod (%)
uint64_t __umoddi3(uint64_t a, uint64_t b) {
    return uint64_divmod(a, b).rem;
}

int64_t __moddi3(int64_t a, int64_t b) {
    return uint64_divmod(a, b).rem;
}

// TODO: add more compilers support

}