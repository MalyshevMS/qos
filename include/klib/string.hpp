#pragma once

#include <cstddef>

namespace kstd {
class string {
private:
    char* head;
    size_t len;
public:
    static const size_t npos = -1;

    string();
    string(const char* cstr);
    string(const char* data, size_t size);
    
    char& operator[](size_t index) const;
    
    friend string operator+(const string& left, const string& right);
    friend string operator+(const string& left, const char* right);
    friend string operator+(const char* left, const string& right);
    friend string operator+(const string& left, char right);
    friend string operator+(char left, const string& right);

    string& operator+=(const string& str);
    string& operator+=(const char* str);
    string& operator+=(char ch);

    string& operator=(const string& other);

    size_t find(char seek, size_t from = 0) const;

    string substr(size_t from, size_t size = npos) const;

    const char* c_str() const;
    const char* data() const { return c_str(); };

    friend bool operator==(const string& left, const string& right);

    size_t size() const { return len; }

    // void reserve(size_t size); // TODO: implement a capacity
};

string operator+(const string& left, const string& right);
string operator+(const string& left, const char* right);
string operator+(const char* left, const string& right);
string operator+(const string& left, char right);
string operator+(char left, const string& right);
bool operator==(const string& left, const string& right);
} // namespace std