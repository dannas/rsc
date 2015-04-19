#pragma once

#include <cstring>
#include <iostream> // DEBUG

static int roundup(int x, int multiple)
{
    return (x + multiple - 1) & ~(multiple - 1);
}

class String
{
public:
    String();
    String(const char *t);
    String(const String &t);
    ~String();

    String& operator=(const String &t);

    void append(const String &t);
    void append(const char *t);

private:
    void resize(int max);
    char *sp;
    int N;
    int cap;
};

String::String()
{
    cap = 64;
    N = 0;
    resize(cap);
    *sp = '\0';
}

String::String(const char *t)
{
    cap = 64;
    N = 0;
    resize(cap);
    append(t);
}

String::String(const String &t)
{
    cap = 64;
    N = 0;
    resize(cap);
    append(t);
}

String::~String()
{
    delete[] sp;
}

String& String::operator=(const String &t)
{
    if (this != &t) {
        delete[] sp;
        N = cap = 0;
        append(t);
    }
    return *this;
}

void String::append(const String &t)
{
    //std::cerr << __func__ << " " << N << " " << cap << "\n";
    int max = t.N + 1;
    if (max+N > cap) {
        max = roundup(max+N, 64);
        resize(max);
    }
    for (int i = N; i < N + t.N; i++)
        sp[i] = t.sp[i];
    N = t.N;
}

void String::append(const char *t)
{
    int max = strlen(t) + 1;
    if (max > cap) {
        max = roundup(max, 64);
        resize(max);
    }
    for (int i = N; i < N + max; i++)
        sp[i] = t[i];
    N = max-1;
}


void String::resize(int max)
{
    //std::cerr << "     " << __func__ << " " << max << "\n";
    char *t = new char[max];
    cap = max;

    for (int i = 0; i < N; i++)
        t[i] = sp[i];
    t[N] = '\0';
    sp = t;
}
