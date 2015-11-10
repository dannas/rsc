#include "util.h"

void die(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, ": %s\n", strerror(errno));
    exit(1);
}

pid_t xfork()
{
    pid_t pid = fork();
    if (pid < 0)
        die("fork");
    return pid;
}

long xptrace(enum __ptrace_request request, pid_t pid,
             void *addr, void *data)
{
    errno = 0;
    long ret = ptrace(request, pid, addr, data);

    // The PTRACE_PEEK* requests returns data, which may be -1. 
    if (ret == -1 && errno) {
        die("#### ptrace");
    }
    return ret;
}

void xexecvp(const char* file, char* const argv[])
{
    int ret = execvp(file, argv);
    if (ret < 0) {
        die("execvp");
    }
}

void xkill(pid_t pid, int sig)
{
    if (-1 == kill(pid, sig))
        die("kill");
}

void xpersonality(unsigned long persona)
{
    if (-1 == personality(persona))
        die("personality");
}
