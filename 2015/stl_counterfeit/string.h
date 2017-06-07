#pragma once

#include <stddef.h>

#include <cassert>

namespace danstd {

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

    size_t capacity() {
        return cap_;
    }

    size_t size() {
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

private:

    void checkRep() {
        assert(len_ <= cap_);
        assert(buf_ != nullptr);
    }

    char *buf_;     // The actual string in memory.
    size_t len_;    // Number of bytes occupied, excluding trailing '\0' character.
    size_t cap_;    // Number of bytes allocated.
};

}
