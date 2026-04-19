#pragma once

#include <cstddef>

namespace kstd {
class string {
private:
    char* data_;
    size_t len;
    size_t capacity;

    void reallocate(size_t new_capacity);
    void grow_if_needed(size_t required_size);

public:
    static const size_t npos = -1;

    string();
    ~string();
    
    string(const char* cstr);
    string(const char* data, size_t size);
    string(const string& other);
    string(char ch);
    
    char& operator[](size_t index);
    const char& operator[](size_t index) const;
    
    string& operator=(const string& other);
    string& operator+=(const string& str);
    string& operator+=(const char* str);
    string& operator+=(char ch);

    const char* c_str() const { return data_; }
    const char* data() const { return data_; }
    
    size_t size() const { return len; }
    size_t length() const { return len; }
    bool empty() const { return len == 0; }
    
    void clear();
    void push_back(char ch);
    char& back();
    const char& back() const;
    
    size_t find(const char* seek, size_t from = 0) const;
    size_t find(char seek, size_t from = 0) const;
    
    string substr(size_t from = 0, size_t count = npos) const;
    string& erase(size_t from = 0, size_t count = npos);
    
    friend bool operator==(const string& left, const string& right);
    friend bool operator!=(const string& left, const string& right);
    friend bool operator<(const string& left, const string& right);
    
    friend string operator+(const string& left, const string& right);
    friend string operator+(const string& left, const char* right);
    friend string operator+(const char* left, const string& right);
    friend string operator+(const string& left, char right);
    friend string operator+(char left, const string& right);
};

string operator+(const string& left, const string& right);
string operator+(const string& left, const char* right);
string operator+(const char* left, const string& right);
string operator+(const string& left, char right);
string operator+(char left, const string& right);

bool operator==(const string& left, const string& right);
bool operator!=(const string& left, const string& right);
bool operator<(const string& left, const string& right);

string to_string(int val);
string to_string(unsigned int val);
string to_string(short val);
string to_string(unsigned short val);
string to_string(long val);
string to_string(unsigned long val);
string to_string(long long val);
string to_string(unsigned long long val);

} // namespace kstd