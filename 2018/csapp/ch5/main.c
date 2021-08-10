#include <stdio.h>
#include <string.h>

__attribute__((noinline)) char* lower1(char *str) {
    for (size_t i = 0; i < strlen(str); i++) {
        if (str[i] >= 'A' && str[i] <= 'Z') {
            str[i] -= ('A' - 'a');
        }
    }
    return str;
}

int main(int argc, char *argv[]) {
    const char buf[] = "Hello World";
    printf("%s\n", lower1(buf));
}
