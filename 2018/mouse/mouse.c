#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

#define PUSH(val) \
    (*top++ = (val))
#define POP(val) \
    (*--top)
#define POPS(n) \
    (assert(top - stack >= n))
#define PUSHES(n) \
    (assert(top + (n) <= stack + MAX_STACK))
#define BINARY_OP(op) \
    do { \
        POPS(2); \
        int right = POP(); \
        int left = POP(); \
        PUSHES(1); \
        PUSH(left op right); \
    } while (0)

// Interpreter for the Mouse language.
// http://mouse.davidgsimpson.com/mouse83/intro83.html
// http://mouse.davidgsimpson.com/mouse83/ref83.html
//
// TODO:
//  Add conditionals [...]
//  Add looping      (...)
//  Add tracing      {...}
//  Add comments     ~....
//  Add macros

int interpret(char *program) {
    enum { MAX_STACK = 1024 };
    int32_t stack[MAX_STACK];
    int32_t *top =stack;

    int32_t registers[25] = {0};

    char *code = program;

    while (*code) {
        int c = *code;
        switch (c) {
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            code++;
            break;
        case '0'...'9': {
            PUSHES(1);
            int32_t val = 0;
            while (isdigit(*code)) {
                val *= 10;
                val += *code - '0';
                code++;
            }
            PUSH(val);
            break;
        }
        case 'A'...'Z':
            PUSHES(1);
            PUSH(c - 'A');
            code++;
            break;
        case ':': {
            POPS(2);
            int32_t right = POP();
            int32_t left = POP();
            registers[right] = left;
            code++;
            break;
        }
        case '.': {
            POPS(1);
            int32_t address = POP();
            assert(address >= 0 && address <= 25);
            PUSH(registers[address]);
            code++;
            break;
        }
        case '+':
            BINARY_OP(+);
            code++;
            break;
        case '-':
            BINARY_OP(-);
            code++;
            break;
        case '*':
            BINARY_OP(*);
            code++;
            break;
        case '/':
            BINARY_OP(/);
            code++;
            break;
        case '\\':
            BINARY_OP(%);
            code++;
            break;
        case '<':
            BINARY_OP(<);
            code++;
            break;
        case '=':
            BINARY_OP(=);
            code++;
            break;
        case '>':
            BINARY_OP(>);
            code++;
            break;
        case '\'':
            code++;
            PUSHES(1);
            if (*code) {
                PUSH(*code);
                code++;
            }
            break;
        case '?':
            code++;
            PUSHES(1);
            if (*code == '\'') {
                PUSH(getchar());
                code++;
            } else  {
                int32_t val;
                scanf("%d", &val);
                PUSH(val);
            }
            break;
        case '!':
            code++;
            POPS(1);
            if (*code == '\'') {
                putchar(POP());
                code++;
            } else {
                printf("%d", POP());
            }
            break;
        case '"': 
            code++;
            while (*code && *code != '"') {
                if (*code == '!') {
                    putchar('\n');
                } else {
                    putchar(*code);
                }
                code++;
            }
            if (*code) {
                code++;
            }
            break;
        case '$':
            goto out;
            break;
        default:
            assert(0);
        }
    }
out:
    return POP();
}


int main(int argc, char *argv[])
{
    assert(1 == interpret("1 1 + 3 * 3 * 9 / 2 + 3 \\"));
    assert(1 == interpret("1 2 <"));
    assert(0 == interpret("1 2 >"));
    interpret("\"abc!def!\" 'x !' \"!\"$");
    interpret("10 A:  A. !");
    return 0;
}
