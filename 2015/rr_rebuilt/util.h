#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <string>

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cerrno>
#include <cstring> // strerror

#include <sys/personality.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/reg.h> // Constants ORIG_RAX, RAX, ...
#include <sys/wait.h>
#include <unistd.h>

/**
 * Converts a vector of strings to a POSIX-style array of char*s terminated
 * by a nullptr.
 */
class StringVectorToCharArray {
public:
  StringVectorToCharArray(const std::vector<std::string>& vs) {
    for (auto& v : vs) {
      array.push_back(const_cast<char*>(v.c_str()));
    }
    array.push_back(nullptr);
  }
  char** get() { return array.data(); }

private:
  std::vector<char*> array;
};

void die(const char* fmt, ...);

pid_t xfork();

long xptrace(enum __ptrace_request request, pid_t pid, void* addr, void* data);

void xexecvp(const char* file, char* const argv[]);

void xkill(pid_t pid, int sig);

void xpersonality(unsigned long persona);

#endif
