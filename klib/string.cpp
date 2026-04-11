#include <klib/string.hpp>
#include <klib/cstring.hpp>
#include <klib/mem.hpp>

kstd::string::string() : head(nullptr), len(0) {}

kstd::string::string(const char* cstr) {
    if (!cstr) {
        head = nullptr;
        len = 0;
        return;
    }
    len = strlen(cstr);
    head = new char[len];
    memcpy(head, cstr, len);
}

kstd::string::string(const char *data, size_t size) {
    len = size;
    head = new char[len];
    memcpy(head, data, len);
}

char& kstd::string::operator[](size_t index) const {
    if (index > len) [[unlikely]] return *head;
    return head[index];
}

const char* kstd::string::c_str() const {
    auto res = new char[len + 1];
    memcpy(res, head, len);
    res[len] = 0; // Null terminator
    return res;
}

size_t kstd::string::find(char seek, size_t from) const {
    for (int i = from; i < len; i++) if (head[i] == seek) return i;
    return npos;
}

kstd::string kstd::string::substr(size_t from, size_t size) const {
    return string(head + from, size);
}

kstd::string kstd::operator+(const string &left, const string &right) {
    string res;
    res.len = left.len + right.len;
    res.head = new char[res.len];

    memcpy(res.head, left.head, left.len); // Copy first string

    for (int i = 0; i < right.len; i++) { // Copy second string
        res.head[i + left.len] = right[i];
    }

    return res;
}

kstd::string kstd::operator+(const string &left, const char *right) {
    string res;
    auto rlen = strlen(right);
    res.len = left.len + rlen;
    res.head = new char[res.len];

    memcpy(res.head, left.head, left.len);

    for (int i = 0; i < rlen; i++) {
        res.head[i + left.len] = right[i];
    }

    return res;
}

kstd::string kstd::operator+(const char *left, const string &right) {
    string res;
    auto llen = strlen(left);
    res.len = llen + right.len;
    res.head = new char[res.len];

    memcpy(res.head, left, llen);

    for (int i = 0; i < right.len; i++) {
        res.head[i + llen] = right[i];
    }

    return res;
}

kstd::string kstd::operator+(const string &left, char right) {
    string res;
    res.len = left.len + 1;
    res.head = new char[res.len];
    memcpy(res.head, left.head, left.len);
    res.head[left.len] = right;

    return res;
}

kstd::string kstd::operator+(char left, const string &right) {
    string res;
    res.len = right.len + 1;
    res.head = new char[res.len];
    res.head[0] = left;

    for (int i = 0; i < right.len; i++) res.head[i + 1] = right[i];

    return res;
}

kstd::string& kstd::string::operator+=(const string &str) {
    *this = *this + str;
    return *this;
}

kstd::string& kstd::string::operator+=(const char *str) {
    *this = *this + str;
    return *this;
}

kstd::string& kstd::string::operator+=(char ch) {
    *this = *this + ch;
    return *this;
}

bool kstd::operator==(const string &left, const string &right) {
    for (int i = 0; i < left.len; i++) if (left[i] != right[i]) return false;
    return true;
}

kstd::string& kstd::string::operator=(const string &other)
{
    delete head;
    len = other.len;
    head = new char[len];
    memcpy(head, other.head, len);
    return *this;
}