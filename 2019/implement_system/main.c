#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include <grp.h> // setgroup
#include <dirent.h>

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

// Not async-signal safe. opendir(3) allocates memory.
int system4(const char *cmd) {
    int status;
    pid_t pid;

    switch (pid = fork()) {
    case -1:
        return -1;
    case 0: {
        DIR *dir = opendir("/proc/self/fd");
        if (dir == NULL) {
            perror("opendir");
            abort();
        }

        struct dirent *dirent = NULL;
        errno = 0;
        dirent = readdir(dir);
        do {
            if (dirent == NULL && errno != 0) {
                perror("readdir");
                abort();
            }

            if (dirent->d_type & DT_UNKNOWN) {
                fprintf(stderr, "File type could not be determined\n");
                abort();
            }

            if (dirent->d_type & DT_LNK) {
                errno = 0;
                long val = strtol(dirent->d_name, NULL, 10);
                if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
                        || (errno != 0 && val == 0)) {
                    fprintf(stderr, "Unknown file descriptor\n");
                    abort();
                }
                int fd = val;

                if (fd > 2 && fd != dirfd(dir)) {
                    close(fd);
                }
            }
            dirent = readdir(dir);
        } while (dirent != NULL);

        closedir(dir);

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

typedef struct LinuxDirent {
    long ino;
    off_t off;
    unsigned short len;
    char name[];
} LinuxDirent;

int system5(const char *cmd) {
    int status;
    pid_t pid;

    switch (pid = fork()) {
    case -1:
        return -1;
    case 0: {
        int dir_fd = open("/proc/self/fd", O_DIRECTORY | O_RDONLY);
        char buf[BUFSIZ];

        while (true) {
            ssize_t num_read = syscall(SYS_getdents, dir_fd, buf, sizeof(buf));
        }


        // TODO: check error



        close(dir_fd);
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

int system6(const char *cmd) {
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
#if 0
    int fd = open("/proc/self/fd", O_DIRECTORY, O_RDONLY);
    char buf[BUFSIZ];
    while (true) {
        int num_read = syscall(SYS_getdents, fd, buf, BUFSIZ);
        if (num_read == 0) {

            break;
        }
    }
#endif


    extern char** environ;
    size_t environ_size = 0;
    size_t num_bytes = 0;
    for (char **ptr = environ; *ptr != NULL; ptr++) {
        environ_size++;
        num_bytes += strlen(*ptr) + 1;
    }

    char *env_buf = malloc(num_bytes);
    char *new_environ = calloc(environ_size, sizeof(*environ));

    const char **restriced_environ = {
        "PATH",
        "IFS",
        NULL
    };

    const char **preserved_environ = {
        "TZ",
        NULL
    };

    for (char **ptr = environ; *ptr != NULL; ptr++) {
        char *key = *ptr;
        char *end = strchr(*ptr, '=');
        if  (!end) {
            continue;
        }
        char *val = end + 1;
        printf("%.*s = %s \n", end-key, key, val);

    }
}
