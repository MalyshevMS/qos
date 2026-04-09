#pragma once

#include <klib/cstring.hpp>
#include <klib/string.hpp>

#define FMT_TEMPLATE template<typename... Args>
#define FMT_ARGS const char* format, Args... args
#define FMT(fsig) FMT_TEMPLATE fsig(FMT_ARGS)

namespace kstd {
inline void format_helper(string& res, const char* fmt) {
    while (*fmt) {
        if (*fmt == '{' && *(fmt + 1) == '}') fmt += 2;
        else                                  res += (*fmt++);
    }
}

// Overloads here
inline void append_arg(string& res, int val) {
    char buf[12]; 
    itoa(val, buf, 10);
    res += buf;
}

inline void append_arg(string& res, const string& val) {
    res += val;
}

inline void append_arg(string& res, const char* val) {
    res += val;
}

inline void append_arg(string& res, char val) {
    res += val;
}

template<typename T, typename... Args>
void format_helper(string& res, const char* fmt, T first, Args... args) {
    while (*fmt) {
        if (*fmt == '{' && *(fmt + 1) == '}') {
            append_arg(res, first);
            format_helper(res, fmt + 2, args...);
            return;
        } else res += (*fmt++);
    }
}

template<typename... Args>
string fmt(const char* fmt, Args... args) {
    string res;
    format_helper(res, fmt, args...);
    return res;
}
} // namespace kstd