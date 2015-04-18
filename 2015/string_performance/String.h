#pragma once

#include <cstring>

static int roundup(int x, int multiple)
{
    return (x + multiple - 1) & ~(multiple - 1);
}

class String
{
public:
    String();
    String(const String &t);
    ~String();
    // TODO(dannas): Rule-of-tree. Add assignment operator.

    void append(const String &t);

private:
    void resize(int max);
    char *sp;
    int N;
    int cap;
};

String::String()
{
    cap = 64;
    resize(cap);
    *sp = '\0';
    N = 0;
}

String::String(const String &t)
{
    cap = 64;
    resize(cap);
    N = 0;
    append(t);
}

String::~String()
{
    delete[] sp;
}

void String::append(const String &t)
{
    int max = t.N + 1;
    if (max > cap) {
        max = roundup(max, 64);
        resize(max);
    }
    for (int i = N; i < N + t.N; i++)
        sp[i] = t.sp[i];
    N = t.N;
}

void String::resize(int max)
{
    char *t = new char[max];
    cap = max;

    for (int i = 0; i < N; i++)
        t[i] = sp[i];
    t[N] = '\0';
    sp = t;
}
