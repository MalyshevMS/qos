#include <klib/string.hpp>
#include <klib/cstring.hpp>
#include <klib/mem.hpp>

void kstd::string::reallocate(size_t new_capacity) {
    if (new_capacity == 0) {
        delete[] data_;
        data_ = nullptr;
        capacity = 0;
        return;
    }

    char* new_data = new char[new_capacity];
    if (data_) {
        memcpy(new_data, data_, len);
        delete[] data_;
    }
    data_ = new_data;
    capacity = new_capacity;
}

void kstd::string::grow_if_needed(size_t required_size) {
    if (required_size >= capacity) {
        size_t new_capacity = capacity == 0 ? 16 : capacity * 2;
        while (new_capacity <= required_size) {
            new_capacity *= 2;
        }
        reallocate(new_capacity);
    }
}

kstd::string::string() : data_(nullptr), len(0), capacity(0) {
    grow_if_needed(1);
    data_[0] = '\0';
}

kstd::string::~string() {
    if (data_) {
        delete[] data_;
        data_ = nullptr;
    }
}

kstd::string::string(const char* cstr) : data_(nullptr), len(0), capacity(0) {
    if (!cstr) {
        grow_if_needed(1);
        data_[0] = '\0';
        return;
    }
    
    len = strlen(cstr);
    grow_if_needed(len + 1);
    memcpy(data_, cstr, len);
    data_[len] = '\0';
}

kstd::string::string(const char* src, size_t size) : data_(nullptr), len(size), capacity(0) {
    grow_if_needed(len + 1);
    if (src) {
        memcpy(data_, src, len);
    }
    data_[len] = '\0';
}

kstd::string::string(const string& other) : data_(nullptr), len(other.len), capacity(0) {
    grow_if_needed(len + 1);
    memcpy(data_, other.data_, len);
    data_[len] = '\0';
}

kstd::string::string(char ch) : data_(nullptr), len(1), capacity(0) {
    grow_if_needed(2);
    data_[0] = ch;
    data_[1] = '\0';
}

char& kstd::string::operator[](size_t index) {
    return data_[index];
}

const char& kstd::string::operator[](size_t index) const {
    return data_[index];
}

kstd::string& kstd::string::operator=(const string& other) {
    if (this != &other) {
        len = other.len;
        grow_if_needed(len + 1);
        memcpy(data_, other.data_, len);
        data_[len] = '\0';
    }
    return *this;
}

kstd::string& kstd::string::operator+=(const string& str) {
    size_t new_len = len + str.len;
    grow_if_needed(new_len + 1);
    memcpy(data_ + len, str.data_, str.len);
    len = new_len;
    data_[len] = '\0';
    return *this;
}

kstd::string& kstd::string::operator+=(const char* str) {
    if (!str) return *this;
    size_t slen = strlen(str);
    size_t new_len = len + slen;
    grow_if_needed(new_len + 1);
    memcpy(data_ + len, str, slen);
    len = new_len;
    data_[len] = '\0';
    return *this;
}

kstd::string& kstd::string::operator+=(char ch) {
    grow_if_needed(len + 2);
    data_[len] = ch;
    data_[++len] = '\0';
    return *this;
}

void kstd::string::clear() {
    len = 0;
    if (data_) data_[0] = '\0';
}

void kstd::string::push_back(char ch) {
    *this += ch;
}

char& kstd::string::back() {
    return data_[len - 1];
}

const char& kstd::string::back() const {
    return data_[len - 1];
}

size_t kstd::string::find(const char* seek, size_t from) const {
    if (!seek || from >= len) return npos;
    
    size_t seek_len = strlen(seek);
    for (size_t i = from; i <= len - seek_len; i++) {
        bool match = true;
        for (size_t j = 0; j < seek_len; j++) {
            if (data_[i + j] != seek[j]) {
                match = false;
                break;
            }
        }
        if (match) return i;
    }
    return npos;
}

size_t kstd::string::find(char seek, size_t from) const {
    for (size_t i = from; i < len; i++) {
        if (data_[i] == seek) return i;
    }
    return npos;
}

kstd::string kstd::string::substr(size_t from, size_t count) const {
    if (from >= len) return string();
    
    size_t actual_count = count;
    if (count == npos || from + count > len) {
        actual_count = len - from;
    }
    
    return string(data_ + from, actual_count);
}

kstd::string& kstd::string::erase(size_t from, size_t count) {
    if (from >= len) return *this;
    
    size_t actual_count = count;
    if (count == npos || from + count > len) {
        actual_count = len - from;
    }
    
    if (from + actual_count < len) {
        memcpy(data_ + from, data_ + from + actual_count, len - from - actual_count);
    }
    len -= actual_count;
    data_[len] = '\0';
    
    return *this;
}

kstd::string kstd::operator+(const string& left, const string& right) {
    string res;
    res.len = left.len + right.len;
    res.grow_if_needed(res.len + 1);
    memcpy(res.data_, left.data_, left.len);
    memcpy(res.data_ + left.len, right.data_, right.len);
    res.data_[res.len] = '\0';
    return res;
}

kstd::string kstd::operator+(const string& left, const char* right) {
    if (!right) return left;
    string res;
    size_t rlen = strlen(right);
    res.len = left.len + rlen;
    res.grow_if_needed(res.len + 1);
    memcpy(res.data_, left.data_, left.len);
    memcpy(res.data_ + left.len, right, rlen);
    res.data_[res.len] = '\0';
    return res;
}

kstd::string kstd::operator+(const char* left, const string& right) {
    if (!left) return right;
    string res;
    size_t llen = strlen(left);
    res.len = llen + right.len;
    res.grow_if_needed(res.len + 1);
    memcpy(res.data_, left, llen);
    memcpy(res.data_ + llen, right.data_, right.len);
    res.data_[res.len] = '\0';
    return res;
}

kstd::string kstd::operator+(const string& left, char right) {
    string res = left;
    res += right;
    return res;
}

kstd::string kstd::operator+(char left, const string& right) {
    string res;
    res.len = 1 + right.len;
    res.grow_if_needed(res.len + 1);
    res.data_[0] = left;
    memcpy(res.data_ + 1, right.data_, right.len);
    res.data_[res.len] = '\0';
    return res;
}

bool kstd::operator==(const string& left, const string& right) {
    if (left.len != right.len) return false;
    for (size_t i = 0; i < left.len; i++) {
        if (left.data_[i] != right.data_[i]) return false;
    }
    return true;
}

bool kstd::operator!=(const string& left, const string& right) {
    return !(left == right);
}

bool kstd::operator<(const string& left, const string& right) {
    size_t min_len = left.len < right.len ? left.len : right.len;
    for (size_t i = 0; i < min_len; i++) {
        if (left.data_[i] < right.data_[i]) return true;
        if (left.data_[i] > right.data_[i]) return false;
    }
    return left.len < right.len;
}