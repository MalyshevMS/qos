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

inline void append_arg_hex(string& res, unsigned int val) {
    char buf[20];  // Increased buffer size for safety
    const char* hex_chars = "0123456789ABCDEF";
    
    buf[0] = '0';
    buf[1] = 'x';
    
    if (val == 0) {
        buf[2] = '0';
        buf[3] = '\0';
    } else {
        int pos = 2;
        unsigned int copy = val;
        int temp_pos = 0;
        char temp_buf[16];
        
        while (copy > 0) {
            temp_buf[temp_pos++] = hex_chars[copy & 0xF];
            copy >>= 4;
        }
        
        for (int i = temp_pos - 1; i >= 0; i--) {
            buf[pos++] = temp_buf[i];
        }
        buf[pos] = '\0';
    }
    
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

// Main template - works with any single argument (but not string - use specialization above)
template<typename T>
void format_helper(string& res, const char* fmt, T first) {
    while (*fmt) {
        if (*fmt == '{' && *(fmt + 1) == '}') {
            append_arg(res, first);
            format_helper(res, fmt + 2);
            return;
        } else if (*fmt == '%' && *(fmt + 1) == 'x') {
            append_arg_hex(res, (unsigned int)first);
            format_helper(res, fmt + 2);
            return;
        } else if (*fmt == '%' && *(fmt + 1) == 'd') {
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
                append_arg_hex(res, (unsigned int)first);
                format_helper(res, fmt + 2, args...);
                return;
            } else if (*(fmt + 1) == 'd') {
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