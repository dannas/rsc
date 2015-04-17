#pragma once

#include <cstring>

// Uses new[] instead of malloc.
char *strdup(const char *s)
{
    char *t = new char[strlen(s)+1];
    strcpy(t, s);
    return t;
}


class String
{
public:
    String();
    ~String();
    String(const char *t);
    String(const String &t);

    String& operator=(const char *t);
    String& operator=(const String &t);

private:
    char *sp;
};

String::String()
{
    sp = strdup("");
}

String::~String()
{
    delete[] sp;
}

String::String(const char* t)
{
    sp = strdup(t);
}

String::String(const String &t)
{
    sp = strdup(t.sp);
}

String& String::operator=(const char *t)
{
    if (sp != t) {
        delete[] sp;
        sp = strdup(t);
    }
    return *this;

}

String& String::operator=(const String &t)
{
    if (this != &t) {
        delete[] sp;
        sp = strdup(t.sp);
    }
    return *this;
}
