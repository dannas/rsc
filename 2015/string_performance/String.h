#pragma once

#include <cstring>
#include <iostream> // DEBUG
#include <algorithm>

static int roundup(int x, int multiple)
{
    return (x + multiple - 1) & ~(multiple - 1);
}

namespace v1 {
class string
{
public:
    string();
    string(const char *t);
    string(const string &t);
    ~string();

    void append(const string &t);

private:
    void reserve(int max);
    char *buf;
    int len;
    int cap;

};

string::string()
    : buf(0), cap(0), len(0)
{
}

string::string(const char *t)
{
    cap = len = strlen(t);
    buf = new char[cap];
    std::copy(t, t + len, buf);
}

string::string(const string &t)
    : buf(new char[t.cap]),
      cap(t.cap),
      len(t.len)
{
    std::copy(t.buf, t.buf+len, buf);
}

string::~string()
{
    delete[] buf;
}

void string::append(const string &t)
{
    reserve(len + t.len);
    std::copy(t.buf, t.buf+t.len, buf+len);
    len += t.len;
}

void string::reserve(int n)
{
    if (cap < n) {
        n = std::max(2*cap, n);
        char *t = new char[n];
        std::copy(buf, buf+len, t);
        delete[] buf;
        buf = t;
        cap = n;
    }
}
} // namespace v1

namespace v2 {

class string
{
public:
    string();
    string(const char *t);
    string(const string &t);
    ~string();

    void append(const string &t);
private:
    void reserve(int max);

    union {
        char *buf;
        char s[24];
    };
    int len;
    int cap;
};

string::string()
    : buf(0), len(0), cap(24)
{
}

string::string(const char *t)
{
    cap = len = strlen(t);
    if (len <= 24) {
        std::copy(t, t+len, s);
    } else {
        buf = new char[cap];
        std::copy(t, t + len, buf);
    }
}

string::string(const string &t)
{
    len = t.len;
    cap = t.cap;
    if (len <= 24) {
        std::copy(t.s, t.s+len, s);
    } else {
        buf = new char[cap];
        std::copy(t.buf, t.buf+len, buf);
    }
}

string::~string()
{
    if (len >= 24)
        delete[] buf;
}

void string::append(const string &t)
{
    reserve(len + t.len);

    // TODO(dannas): Use iterators for hiding this mess
    // We need to both keep track of whether our string
    // uses the stack or heap and what the other string does.
    if (len + t.len <= 24) {
        std::copy(t.s, t.s+t.len, s+len);
    } else {
        if (t.len <= 24)
            std::copy(t.s, t.s+t.len, buf+len);
        else
            std::copy(t.buf, t.buf+t.len, buf+len);
    }

    len += t.len;
}

void string::reserve(int n)
{
    // enough room in s
    if (len <= 24 && n <= 24)
        return;

    if (cap < n) {
        n = std::max(2*cap, n);
        char *t = new char[n];
        if (cap <= 24)
            std::copy(s, s+len, t);
        else {
            std::copy(buf, buf+len, t);
            delete[] buf;
        }
        buf = t;
        cap = n;
    }
}
} // namespace v2
