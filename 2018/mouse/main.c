#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define POPS(n) \
    (assert(sp - stack >= n))
#define PUSHES(n) \
    (assert(sp + (n) <= stack + MAX_STACK))
#define PUSH(val) \
    (PUSHES(1), *sp++ = (val))
#define POP(val) \
    (POPS(1), *--sp)
#define BINARY_OP(op) \
    do { \
        int32_t right = POP(); \
        int32_t left = POP(); \
        PUSH(left op right); \
    } while (0)

enum {
    MAX_STACK = 32,
    NUM_REGS = 'Z'-'A' + 1,
    NUM_MACROS = NUM_REGS,
    MAX_PARAMS = 10,
    MAX_CALLFRAMES = 10,
    MAX_CONTROLSTACK = 10
};

typedef struct CallFrame {
    char *return_address;
    int32_t num_params;
    int32_t params[MAX_PARAMS];
    int32_t saved_regs[NUM_REGS];
} CallFrame;

char *code = NULL;

// Interpreter for the Mouse language.
// http://mouse.davidgsimpson.com/mouse83/intro83.html
// http://mouse.davidgsimpson.com/mouse83/ref83.html
//
//
//  Add tracing      {...}
//  Decide if $ should push '1' if stack is empty. Useful for testing
//  Decide if we should have bounds checks or just rely on $ for end of program
//  Add error reporting
//  Decide how to handle registers. Which are caller/callee saved?

int32_t scan_int() {
    assert(isdigit(*code));
    int32_t val = 0;
    while (isdigit(*code)) {
        val *= 10;
        val += *code - '0';
        code++;
    }
    return val;
}

void eat(char c) {
    while (*code && *code != c) {
        code++;
    }
    if (*code) {
        code++;
    }
}

int32_t interpret(char *program) {
    int32_t stack[MAX_STACK];
    int32_t *sp = stack;

    char *controlStack[MAX_CONTROLSTACK];
    char **cp = controlStack;

    CallFrame callstack[MAX_CALLFRAMES];
    CallFrame *fp = callstack;

    int32_t registers[NUM_REGS] = {0};
    char *macros[NUM_MACROS] = {NULL};

    code = program;

    while (*code) {
        if (*code == '$') {
            code++;
            if (isalpha(*code)) {
                char c = *code;
                code++;
                assert((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
                macros[tolower(c)- 'a'] = code;
            }
        } else {
            code++;
        }
    }

    code = program;
    while (*code) {
        switch (*code) {
        case ' ':
        case '\t':
        case '\n':
        case '\r':
        case '\v':
            code++;
            break;
        case '~':
            eat('\n');
            break;
        case '0'...'9': {
            int32_t val = scan_int();
            PUSH(val);
            break;
        }
        case 'a'...'z':
        case 'A'...'Z':
            PUSH(tolower(*code) - 'a');
            code++;
            break;
        case ':': {
            int32_t right = POP();
            int32_t left = POP();
            assert(right >= 0 && right < NUM_REGS);
            registers[right] = left;
            code++;
            break;
        }
        case '.': {
            int32_t address = POP();
            assert(address >= 0 && address < NUM_REGS);
            PUSH(registers[address]);
            code++;
            break;
        }
        case '[':
            if (!POP()) {
                eat(']');
            } else {
                code++;
            }
            break;
        case ']':
            code++;
            break;
        case '(':
            code++;
            assert(cp >= controlStack && cp < controlStack + MAX_CONTROLSTACK);
            *cp++ = code;
            break;
        case ')':
            assert(cp >= controlStack && cp < controlStack + MAX_CONTROLSTACK);
            code = *(cp-1);
            break;
        case '^':
            if (!POP()) {
                eat(')');
                cp--;
            } else {
                code++;
            }
            break;
        case '#':
            code++;
            fp++;
            memset(fp, 0, sizeof(*fp));
            char name = *code;
            assert((name >= 'a' && name <= 'z') || (name >= 'A' && name <= 'Z'));

            while (*code && *code != ';') {
                if (isdigit(*code)) {
                    int32_t val = scan_int();
                    fp->params[fp->num_params] = val;
                    fp->num_params++;
                } else {
                    code++;
                }
            }
            if (*code) {
                code++;
            }
            fp->return_address = code;
            memcpy(fp->saved_regs, registers, NUM_REGS);
            code = macros[tolower(name) - 'a'];

            break;
        case '@':
            memcpy(registers, fp->saved_regs, NUM_REGS);
            assert(fp >= callstack);
            code = fp->return_address;
            fp--;
            break;
        case '%': {
            code++;
            int32_t index = POP();
            assert(index > 0 && index <= fp->num_params);
            PUSH(fp->params[index-1]);
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
            BINARY_OP(==);
            code++;
            break;
        case '>':
            BINARY_OP(>);
            code++;
            break;
        case '\'':
            code++;
            if (*code) {
                PUSH(*code);
                code++;
            }
            break;
        case '?':
            code++;
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
            if (sp == stack) {
                PUSH(1);
            }
            goto out;
            break;
        default:
            assert(0);
        }
    }
out:
    return POP();
}

#undef PUSH
#undef POP
#undef POPS
#undef PUSHES
#undef BINARY_OP

#define assert_interpret(program, expected) \
    assert(interpret(program) == expected)

void test_interpret() {
    // Arithmetics
    assert_interpret("1 1 + 3 * 3 * 9 / 2 + 3 \\", 1);

    // Compares
    assert_interpret("1 2 <", 1);
    assert_interpret("1 2 >", 0);
    assert_interpret("1 1 =", 1);

    assert_interpret("1 ~a comment\n", 1);
    assert_interpret("1 ~ a comment\n", 1);
    assert_interpret("1 ~ a comment", 1);
    assert_interpret("1 ~ a comment~", 1);

    // Registers
    assert_interpret("10 A:  A.", 10);

    // Conditionals
    assert_interpret("3 1 [ 5 ] $", 5);
    assert_interpret("3 0 [ 5 ] $", 3);

    // Iteration
    assert_interpret("(a.  1 +  a: a. 10 < ^) a. ~ while (sum < 0) sum++;", 10);

    // Strings
    interpret("\"abc!def!\" 'x !' \"!\"$");

    // Macros
    assert_interpret("#a; $a 1 @", 1);
    assert_interpret("#a,2; $a 1% @", 2);

}

#undef assert_interpret

char *read_file(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long len = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buf = malloc(len + 1);
    if (len && fread(buf, len, 1, file) != 1) {
        fclose(file);
        free(buf);
        return NULL;
    }
    fclose(file);
    buf[len] = '\0';
    return buf;
}

int main() {
    test_interpret();

    char *program = read_file("../test.mouse");
    if (!program) {
        perror("read_file");
        exit(1);
    }

    interpret(program);

    return 0;
}

