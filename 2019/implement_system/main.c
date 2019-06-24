#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include <grp.h> // setgroup

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <sys/syscall.h>

#ifndef OPEN_MAX
#define OPEN_MAX 1024
#endif

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

int system3(const char *cmd) {
    int status;
    pid_t pid;

    switch (pid = fork()) {
    case -1:
        return -1;
    case 0: {

        // Determine max number of open files.
        // _SC_OPEN_MAX is bound by the resource limit RT_NOFILE and may
        // change during runtime.
        int num_fd = sysconf(_SC_OPEN_MAX);
        if (num_fd ==-1) {
            num_fd = OPEN_MAX;
        }

        for (int fd = 3; fd < num_fd; fd++) {
            close(fd);
        }

        execl("/bin/sh", "sh", "-c", (char **)cmd, NULL);
        _exit(127);
    }

    default:
        if (waitpid(pid, &status, 0) == -1) {
            return -1;
        }
    }
    return status;
}

int system4(const char *cmd) {
    int status;
    pid_t pid;

    switch (pid = fork()) {
    case -1:
        return -1;
    case 0: {
        gid_t new_gid = getgid();
        gid_t old_gid = getegid();
        uid_t new_uid = getuid();
        uid_t old_uid = geteuid();

        // Drop all ancillary groups.
        setgroups(1, &new_gid);

        // Drop effective group id.
        if (new_gid != old_gid) {
            if (setregid(new_gid, new_gid) == -1) {
                abort();
            }
        }

        // Drop effective user id.
        if (new_uid != old_uid) {
            if (setreuid(new_uid, new_uid) == -1) {
                abort();
            }
        }

        // Verify that the changes took effect.
        if (new_gid != old_gid && getegid() != new_gid) {
            abort();
        }
        if (new_uid != old_uid && geteuid() != new_uid) {
            abort();
        }

        execl("/bin/sh", "sh", "-c", (char **)cmd, NULL);
        _exit(127);
    }

    default:
        if (waitpid(pid, &status, 0) == -1) {
            return -1;
        }
    }
    return status;
}

int main() {
    int fd = open("/proc/self/fd", O_DIRECTORY, O_RDONLY);
    char buf[BUFSIZ];
    while (true) {
        int num_read = syscall(SYS_getdents, fd, buf, BUFSIZ);
        if (num_read == 0) {
            break;
        }

    }


}
