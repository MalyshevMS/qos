#include <cstdint>

extern "C" {

typedef struct {
    uint64_t quot;
    uint64_t rem;
} uint64_div_t;

uint64_div_t uint64_divmod(uint64_t dividend, uint64_t divisor) {
    uint64_div_t result;
    if (divisor == 0) {
        // TODO: Panic here
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

// GCC division (/)
uint64_t __udivdi3(uint64_t a, uint64_t b) {
    return uint64_divmod(a, b).quot;
}

// GCC mod (%)
uint64_t __umoddi3(uint64_t a, uint64_t b) {
    return uint64_divmod(a, b).rem;
}

// TODO: add more compilers support

}