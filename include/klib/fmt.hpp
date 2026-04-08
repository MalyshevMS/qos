#pragma once

#include <klib/string.hpp>
#include <cstdarg>
#include <cstdint>
#include <cstddef>

namespace kstd {
namespace fmt {
inline string int_to_string(int v) {
    if (v == 0) return "0";
    bool neg = v < 0;
    unsigned int u = neg ? -static_cast<unsigned int>(v) : static_cast<unsigned int>(v);
    char buf[12]; // Enough for -2147483648 + null
    char* p = buf + sizeof(buf) - 1;
    *p = '\0';
    while (u > 0) {
        *--p = static_cast<char>('0' + (u % 10));
        u /= 10;
    }
    if (neg) *--p = '-';
    return string(p, buf + sizeof(buf) - 1 - p);
}

// TODO: double (or float) to string

inline string ptr_to_string(const void* p) {
    if (!p) return "0x0";
    constexpr char hex[] = "0123456789abcdef";
    char buf[2 + 2 * sizeof(void*) + 1];
    buf[0] = '0'; buf[1] = 'x';
    auto up = reinterpret_cast<uintptr_t>(p);
    int idx = 2 + 2 * sizeof(void*) - 1;
    buf[idx + 1] = '\0';
    do {
        buf[idx--] = hex[up & 0xF];
        up >>= 4;
    } while (up > 0);
    return string(&buf[0]);
}

// ============================================================================
// Main format function
// ============================================================================
string format(const string& fmt, ...) {
    string out;
    // out.reserve(fmt.size() * 2); // Heuristic reservation // TODO: Uncomment when there is capacity in string class
    va_list args;
    va_start(args, fmt);

    size_t i = 0;
    while (i < fmt.size()) {
        if (fmt[i] == '{') {
            // Handle escape {{
            if (i + 1 < fmt.size() && fmt[i + 1] == '{') {
                out += '{';
                i += 2;
                continue;
            }

            // Find closing }
            size_t end = fmt.find('}', i + 1);
            if (end == string::npos) {
                out += fmt.substr(i);
                break; // Malformed, dump rest
            }

            // Parse specifier
            string spec(fmt.data() + i + 1, end - i - 1);
            char type = 'i'; // default to int
            if (spec == "i" || spec == "d") type = 'i';
            // else if (spec == "f") type = 'f';
            else if (spec == "s") type = 's';
            else if (spec == "b") type = 'b';
            else if (spec == "p") type = 'p';

            // Extract and format
            switch (type) {
                case 'i': out += int_to_string(va_arg(args, int)); break;
                // case 'f': out += double_to_string(va_arg(args, double)); break; // TODO: this
                case 's': {
                    const char* s = va_arg(args, const char*);
                    out += s ? s : "(null)";
                    break;
                }
                case 'b': out += (va_arg(args, int) ? "true" : "false"); break;
                case 'p': out += ptr_to_string(va_arg(args, void*)); break;
                default:  out += "{}"; // Fallback for unknown spec
            }
            i = end + 1;
        } 
        else if (fmt[i] == '}') {
            // Handle escape }}
            if (i + 1 < fmt.size() && fmt[i + 1] == '}') {
                out += '}';
                i += 2;
                continue;
            }
            out += fmt[i++]; // Unmatched }, treat as literal
        } 
        else {
            out += fmt[i++];
        }
    }

    va_end(args);
    return out;
}

} // namespace fmt
} // namespace kstd