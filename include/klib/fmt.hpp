#pragma once

#include <cstdint>
#include <klib/cstring.hpp>
#include <klib/string.hpp>

#define FMT_TEMPLATE template<typename... Args>
#define FMT_ARGS const char* format, Args... args
#define FMT(fsig) FMT_TEMPLATE fsig(FMT_ARGS)

namespace kstd {

// Forward declarations
inline void append_arg(string& res, int val);
inline void append_arg_hex(string& res, unsigned int val);

inline void format_helper(string& res, const char* fmt) {
    while (*fmt) {
        res += (*fmt++);
    }
}

inline void append_arg(string& res, int val) {
    res += to_string(val);
}

inline void append_arg(string& res, unsigned int val) {
    res += to_string(val);
}

inline void append_arg(string& res, short val) {
    res += to_string(val);
}

inline void append_arg(string& res, unsigned short val) {
    res += to_string(val);
}

inline void append_arg(string& res, long val) {
    res += to_string(val);
}

inline void append_arg(string& res, unsigned long val) {
    res += to_string(val);
}

inline void append_arg(string& res, long long val) {
    res += to_string(val);
}

inline void append_arg(string& res, unsigned long long val) {
    res += to_string(val);
}

inline void append_arg(string& res, const string& val) {
    res += val;
}

inline void append_arg(string& res, const char* val) {
    if (val) res += val;
}

inline void append_arg(string& res, char val) {
    res += val;
}

inline void append_arg(string& res, bool val) {
    res += val ? "true" : "false";
}

// inline void append_arg(string& res, unsigned long long val) {
//     char buf[65];
//     ulltoa(val, buf, 10);
//     res += buf;
// }

inline void append_arg_hex(string& res, unsigned int val) {
    char buf[20];
    utoa(val, buf, 16);
    res += "0x";
    res += buf;
}

inline void append_arg_hex(string& res, unsigned long val) {
    char buf[25];
    ultoa(val, buf, 16);
    res += "0x";
    res += buf;
}

// For long long - convert to unsigned long (truncate upper bits)
inline void append_arg_hex(string& res, unsigned long long val) {
    char buf[25];
    ultoa((unsigned long)val, buf, 16);
    res += "0x";
    res += buf;
}

// Specialization for string argument
inline void format_helper(string& res, const char* fmt, const string first) {
    while (*fmt) {
        if (*fmt == '{' && *(fmt + 1) == '}') {
            append_arg(res, first);
            format_helper(res, fmt + 2);
            return;
        } else if (*fmt == '%' && *(fmt + 1) == 's') {
            append_arg(res, first);
            format_helper(res, fmt + 2);
            return;
        } else if (*fmt == '%' && *(fmt + 1) == '%') {
            res += '%';
            fmt += 2;
            continue;
        }
        res += (*fmt++);
    }
}

// Template for single argument (non-string)
template<typename T>
void format_helper(string& res, const char* fmt, T first) {
    while (*fmt) {
        if (*fmt == '{' && *(fmt + 1) == '}') {
            append_arg(res, first);
            format_helper(res, fmt + 2);
            return;
        } else if (*fmt == '%' && *(fmt + 1) == 'x') {
            append_arg_hex(res, (unsigned long)first);
            format_helper(res, fmt + 2);
            return;
        } else if (*fmt == '%' && *(fmt + 1) == 'd') {
            append_arg(res, first);
            format_helper(res, fmt + 2);
            return;
        } else if (*fmt == '%' && *(fmt + 1) == 'u') {
            append_arg(res, first);
            format_helper(res, fmt + 2);
            return;
        } else if (*fmt == '%' && *(fmt + 1) == 's') {
            append_arg(res, first);
            format_helper(res, fmt + 2);
            return;
        } else if (*fmt == '%' && *(fmt + 1) == 'c') {
            append_arg(res, first);
            format_helper(res, fmt + 2);
            return;
        } else if (*fmt == '%' && *(fmt + 1) == 'b') {
            append_arg(res, first);
            format_helper(res, fmt + 2);
            return;
        } else if (*fmt == '%' && *(fmt + 1) == '%') {
            res += '%';
            fmt += 2;
            continue;
        }
        res += (*fmt++);
    }
}

// Template for multiple arguments
template<typename T, typename... Args>
void format_helper(string& res, const char* fmt, T first, Args... args) {
    while (*fmt) {
        if (*fmt == '%' && *(fmt + 1) != '\0' && *(fmt + 1) != '%') {
            if (*(fmt + 1) == 'x') {
                append_arg_hex(res, (unsigned long)first);
                format_helper(res, fmt + 2, args...);
                return;
            } else if (*(fmt + 1) == 'd' || *(fmt + 1) == 'u') {
                append_arg(res, first);
                format_helper(res, fmt + 2, args...);
                return;
            } else if (*(fmt + 1) == 's') {
                append_arg(res, first);
                format_helper(res, fmt + 2, args...);
                return;
            } else if (*(fmt + 1) == 'c') {
                append_arg(res, first);
                format_helper(res, fmt + 2, args...);
                return;
            } else if (*(fmt + 1) == 'b') {
                append_arg(res, first);
                format_helper(res, fmt + 2, args...);
                return;
            }
        } else if (*fmt == '{' && *(fmt + 1) == '}') {
            append_arg(res, first);
            format_helper(res, fmt + 2, args...);
            return;
        } else if (*fmt == '%' && *(fmt + 1) == '%') {
            res += '%';
            fmt += 2;
            continue;
        }
        res += (*fmt++);
    }
}

template<typename... Args>
string fmt(const char* format_str, Args... args) {
    string res;
    format_helper(res, format_str, args...);
    return res;
}

} // namespace kstd