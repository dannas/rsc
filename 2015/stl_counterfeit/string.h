#pragma once

#include <stddef.h>

#include <cassert>

namespace danstd {

// TODO(dannas): Add these methods:
// move constructor
// move assignment operator
// find_first_of
// find_first_not_of
// find
// rfind
// operator+
// comparison operators
class string {
public:
    static const int kDefaultCapacity = 1;

    string()
        : buf_(nullptr)
        , len_(0)
        , cap_(0) {
        reserve(kDefaultCapacity);
        checkRep();
    }

    string(const char* str)
        : string() {
        for (const char *p = str; *p != '\0'; ++p) {
            push_back(*p);
        }
        checkRep();
    }

    string(const string& other) {
        // TODO(dannas): How guarentee exception safety?
        buf_ = new char[other.cap_ + 1];
        for (size_t i = 0; i < other.len_; ++i) {
            buf_[i] = other.buf_[i];
        }
        len_ = other.len_;
        cap_ = other.cap_;
    }

    string& operator=(const string& other) {
        // TODO(dannas): How guarentee exception safety? copy-swap?
        if (this == &other)
            return *this;

        char* temp = new char[other.cap_ + 1];
        for (size_t i = 0; i < other.len_; ++i) {
            temp[i] = other.buf_[i];
        }
        len_ = other.len_;
        cap_ = other.cap_;
        delete[] buf_;
        buf_ = temp;

        return *this;
    }

    ~string() {
        delete[] buf_;
    }

    void reserve(size_t newCap) {
        if (newCap < capacity())
            return;

        // Always reserve one extra char for the null byte terminator.
        char *temp = new char[newCap + 1];

        for (size_t i = 0; i < len_; ++i)
            temp[i] = buf_[i];
        delete[] buf_;

        buf_ = temp;
        cap_ = newCap;
        checkRep();
    }

    size_t capacity() const {
        return cap_;
    }

    size_t size() const {
        return len_;
    }

    char* data() {
        buf_[len_] = '\0';
        return buf_;
    }

    void push_back(char c) {
        if (size() == capacity()) {
            reserve(2 * capacity());
        }
        buf_[len_++] = c;
        checkRep();
    }

    char& operator[](size_t i) {
        return buf_[i];
    }

    char& operator[](size_t i) const {
        return buf_[i];
    }

private:

    void checkRep() {
        assert(len_ <= cap_);
        assert(buf_ != nullptr);
    }

    char *buf_;     // The actual string in memory.
    size_t len_;    // Number of bytes occupied, excluding trailing '\0' character.
    size_t cap_;    // Number of bytes allocated.
};

string operator+(const string& lhs, const string& rhs) {
    string s;
    s.reserve(lhs.size());
    for (size_t i = 0; i < lhs.size(); ++i) {
        s.push_back(lhs[i]);
    }
    for (size_t i = 0; i < rhs.size(); ++i) {
        s.push_back(rhs[i]);
    }
    return s;
}

}
