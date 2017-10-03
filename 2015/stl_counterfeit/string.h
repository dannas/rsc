#pragma once

#include <stddef.h>

#include <cassert>
#include <cstring>
#include <iterator>

namespace danstd {

//
// This class only provides bidirectional iterators to simplify the code.

// TODO(dannas): Add these methods:
// swap
// iterators
// begin and end
class string {
public:
    static const int kDefaultCapacity = 1;

    class iterator {
    public:
        using difference_type = size_t;
        using value_type = char;
        using reference = char&;
        using pointer = char*;
        using iterator_category = std::bidirectional_iterator_tag;

        iterator(char *ptr)
            : ptr_(ptr) {

        }

        iterator& operator++() {
            ++ptr_;
            return *this;
        }

        iterator operator++(int) {
            iterator temp(*this);
            operator++();
            return temp;
        }

        iterator& operator--() {
            --ptr_;
            return *this;
        }

        iterator operator--(int) {
            iterator temp(*this);
            operator--();
            return temp;
        }

        char operator*() {
            return *ptr_;
        }

        bool operator==(const iterator &other) const {
            return ptr_ == other.ptr_;
        }
        bool operator!=(const iterator &other) const {
            return ptr_ != other.ptr_;
        }


    private:
        char *ptr_;

    };

    // Construct an empty string.
    string()
        : buf_(nullptr)
        , len_(0)
        , cap_(0) {
        reserve(kDefaultCapacity);
        checkRep();
    }

    // Construct a string equal to |str|.
    string(const char* str)
        : string() {
        for (const char *p = str; *p != '\0'; ++p) {
            push_back(*p);
        }
        checkRep();
    }

    // Copy construct a string from |other|.
    string(const string& other) {
        buf_ = new char[other.cap_ + 1];
        for (size_t i = 0; i < other.len_; ++i) {
            buf_[i] = other.buf_[i];
        }
        len_ = other.len_;
        cap_ = other.cap_;
    }

    // Move construct a string from |other|.
    string(string&& other) noexcept
        : buf_(other.buf_)
        , len_(other.len_)
        , cap_(other.len_) {
        other.buf_ = nullptr;
        other.len_ = 0;
        other.cap_ = 0;
    }

    // Move assign the content from |other|.
    string& operator=(string&& other) noexcept {
        if (this == &other)
            return *this;

        delete[] buf_;

        buf_ = other.buf_;
        len_ = other.len_;;
        cap_ = other.cap_;

        other.buf_ = nullptr;
        other.len_ = 0;
        other.cap_ = 0;
        return *this;
    }

    // Copy assign the content of |other|.
    string& operator=(const string& other) {
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

    // Destruct the string.
    ~string() {
        delete[] buf_;
    }

    // Reserve |newCap| bytes.
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

    // Return the current number of allocated bytes.
    size_t capacity() const {
        return cap_;
    }

    // Return the size of the string, excluding the '\0' byte.
    size_t size() const {
        return len_;
    }

    // Return the internal null-terminated char array.
    const char* data() const {
        buf_[len_] = '\0';
        return buf_;
    }

    // Add character |c| to the end of the string.
    void push_back(char c) {
        if (size() == capacity()) {
            reserve(2 * capacity());
        }
        buf_[len_++] = c;
        checkRep();
    }

    // Return the character at |index|.
    char& operator[](size_t index) {
        return buf_[index];
    }

    // Return the character at |index|.
    const char& operator[](size_t i) const {
        return buf_[i];
    }

    string& operator+=(const string& other) {
        for (size_t i = 0; i < other.size(); ++i) {
            push_back(other[i]);
        }
        checkRep();
        return *this;
    }

    string& operator+=(const char* str) {
        for (const char *p = str; *p != '\0'; ++p) {
            push_back(*p);
        }
        checkRep();
        return *this;
    }

    iterator begin() {
        return buf_;
    }

    iterator end() {
        return buf_ + len_;
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

string operator+(string lhs, const string& rhs) {
    lhs += rhs;
    return lhs;
}

string operator+(string lhs, const char* str) {
    lhs += str;
    return lhs;
}

bool operator<(const string &lhs, const string &rhs) {
    return strcmp(lhs.data(), rhs.data()) < 0;
}

bool operator==(const string &lhs, const string &rhs) {
    return strcmp(lhs.data(), rhs.data()) == 0;
}

bool operator!=(const string &lhs, const string &rhs) {
    return strcmp(lhs.data(), rhs.data()) != 0;
}

bool operator<=(const string &lhs, const string &rhs) {
    return strcmp(lhs.data(), rhs.data()) <= 0;
}

bool operator>(const string &lhs, const string &rhs) {
    return strcmp(lhs.data(), rhs.data()) > 0;
}

bool operator>=(const string &lhs, const string &rhs) {
    return strcmp(lhs.data(), rhs.data()) >= 0;
}


}
