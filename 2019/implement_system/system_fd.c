#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {
    int fd = open("/etc/passwd", O_RDONLY);
    (void)fd;
    system("./exploit_fd");
}
