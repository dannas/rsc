#include <stdlib.h>
#include <errno.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int system1(const char *cmd) {
    int status;
    pid_t pid;

    switch (pid = fork()) {
    case -1:
        return -1;
    case 0:
        execl("/bin/sh", "sh", "-c", (char **)cmd, NULL);
        _exit(127);

    default:
        if (waitpid(pid, &status, 0) == -1) {
            return -1;
        }
    }
    return status;
}

int system2(const char *cmd) {
    int status;
    pid_t pid;
    sigset_t block_mask;
    sigset_t orig_mask;
    struct sigaction sa_ignore;
    struct sigaction sa_orig_quit;
    struct sigaction sa_orig_int;
    struct sigaction sa_default;
    int saved_errno;

    // Block SIGCHLD
    sigemptyset(&block_mask);
    sigaddset(&block_mask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &block_mask, &orig_mask);

    // Ignore SIGINT and SIGQUIT
    sa_ignore.sa_handler = SIG_IGN;
    sa_ignore.sa_flags = 0;
    sigemptyset(&sa_ignore.sa_mask);
    sigaction(SIGINT, &sa_ignore, &sa_orig_int);
    sigaction(SIGQUIT, &sa_ignore, &sa_orig_quit);

    switch (pid = fork()) {
    case -1:
        status = -1;
        break;
    case 0:
        sa_default.sa_handler = SIG_DFL;
        sa_default.sa_flags = 0;
        sigemptyset(&sa_default.sa_mask);

        if (sa_orig_int.sa_handler != SIG_IGN) {
            sigaction(SIGINT, &sa_default, NULL);
        }
        if (sa_orig_quit.sa_handler != SIG_IGN) {
            sigaction(SIGQUIT, &sa_default, NULL);
        }
        sigprocmask(SIG_SETMASK, &orig_mask, NULL);

        execl("/bin/sh", "sh", "-c", (char **)cmd, NULL);
        _exit(127);
        break;
    default:
        if (waitpid(pid, &status, 0) == -1) {
            return -1;
        }
        break;
    }

    saved_errno = errno;

    sigprocmask(SIG_SETMASK, &orig_mask, NULL);
    sigaction(SIGINT, &sa_orig_int, NULL);
    sigaction(SIGQUIT, &sa_orig_quit, NULL);

    errno = saved_errno;

    return status;
}

int main() {
    system1("sleep 86400");  // Sleep for 24 hours
}
