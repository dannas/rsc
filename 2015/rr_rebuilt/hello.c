// A small testprogram that only executes two syscalls; write and exit.
// Not linked to any library.

#include <sys/types.h>
#include <sys/syscall.h>

int my_write(int fd, const char* buf, size_t count) {
  size_t ret;
  __asm__("syscall\n\t"
          : "=a"(ret)
          : "a"(SYS_write), "D"(fd), "S"(buf), "d"(count));
  return ret;
}

int my_exit(int status) {
  int ret;
  __asm__("syscall\n\t" : "=a"(ret) : "a"(SYS_exit), "D"(status));
  return ret;
}

void _start() {
  const char s[] = "Hello world\n";
  my_write(1, s, sizeof(s) - 1);
  my_exit(0);
}
