#pragma once

#include <klib/cstring.hpp>
#include <klib/string.hpp>

#define FMT_TEMPLATE template<typename... Args>
#define FMT_ARGS const char* format, Args... args
#define FMT(fsig) FMT_TEMPLATE fsig(FMT_ARGS)

namespace kstd {

inline void format_helper(string& res, const char* fmt) {
    while (*fmt) {
        if (*fmt == '{' && *(fmt + 1) == '}') {
            fmt += 2;
        } else {
            res += (*fmt++);
        }
    }
}

inline void append_arg(string& res, int val) {
    char buf[12]; 
    itoa(val, buf, 10);
    res += buf;
}

inline void append_arg(string& res, unsigned int val) {
    char buf[12];
    unsigned int copy = val;
    int pos = 0;
    if (val == 0) {
        buf[0] = '0';
        buf[1] = '\0';
    } else {
        while (copy > 0) {
            buf[pos++] = '0' + (copy % 10);
            copy /= 10;
        }
        buf[pos] = '\0';
        for (int i = 0; i < pos / 2; i++) {
            char tmp = buf[i];
            buf[i] = buf[pos - 1 - i];
            buf[pos - 1 - i] = tmp;
        }
    }
    res += buf;
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

inline void append_arg(string& res, long val) {
    char buf[20];
    itoa((int)val, buf, 10);
    res += buf;
}

template<typename T, typename... Args>
void format_helper(string& res, const char* fmt, T first, Args... args) {
    while (*fmt) {
        if (*fmt == '{' && *(fmt + 1) == '}') {
            append_arg(res, first);
            format_helper(res, fmt + 2, args...);
            return;
        } else {
            res += (*fmt++);
        }
    }
}

template<typename... Args>
string fmt(const char* format_str, Args... args) {
    string res;
    format_helper(res, format_str, args...);
    return res;
}

} // namespace kstd