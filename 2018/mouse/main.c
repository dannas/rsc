#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define POPS(stack, ptr, n) \
    (assert(ptr - stack >= n))
#define PUSHES(stack, ptr, n) \
    (assert(ptr + (n) <= stack + MAX_STACK))

#define PUSH(ptr, val) \
    (*ptr++ = (val))
#define POP(ptr) \
    (*--ptr)
#define TOP(ptr) \
    (*((ptr)-1))

#define BINARY_OP(op) \
    do { \
        POPS(stack, sp, 2); \
        int32_t right = POP(sp); \
        int32_t left = POP(sp); \
        PUSHES(stack, sp, 1); \
        PUSH(sp, left op right); \
    } while (0)

enum {
    MAX_STACK = 64,
    NUM_REGS = 'Z'-'A' + 1,
    NUM_MACROS = NUM_REGS,
    MAX_PARAMS = 10,
};

typedef struct CallFrame {
    char *return_address;
    char *pos_first_param;
    int32_t saved_regs[NUM_REGS];
} CallFrame;

char *code = NULL;

// Interpreter for the Mouse language.
// http://mouse.davidgsimpson.com/mouse83/intro83.html
// http://mouse.davidgsimpson.com/mouse83/ref83.html
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

// Return position of next param or position of ';' or end of string if at end
// TODO(dannas): Clarify what can be returned.
char* next_param() {
    int nesting = 0;

    while (true) {
        if (*code == '#') {
            nesting++;
        } else if (nesting > 0) {
            // TODO(dannas): Should we handle end of string here?
            if (*code == ';') {
                nesting--;
            }
        } else if (*code == ',' || *code == ';' || *code == '\0') {
            break;
        }
        code++;
    }
    assert(nesting == 0);

    if (*code == ',') {
        code++;
    }
    return code;
}

int32_t interpret(char *program) {
    int32_t stack[MAX_STACK];
    int32_t *sp = stack;

    char *controlStack[MAX_STACK];
    char **cp = controlStack;

    CallFrame callstack[MAX_STACK];
    CallFrame *fp = callstack;

    int32_t registers[NUM_REGS] = {0};
    char *macros[NUM_MACROS] = {NULL};

    code = program;

    while (*code) {
        switch (*code) {
        case '~':
            eat('\n');
            break;
        case '$':
            code++;
            if (isalpha(*code)) {
                char c = *code;
                code++;
                assert((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
                macros[tolower(c)- 'a'] = code;
            }
            break;
        default:
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
            PUSHES(stack, sp, 1);
            int32_t val = scan_int();
            PUSH(sp, val);
            break;
        }
        case 'a'...'z':
        case 'A'...'Z':
            PUSHES(stack, sp, 1);
            PUSH(sp, tolower(*code) - 'a');
            code++;
            break;
        case ':': {
            POPS(stack, sp, 2);
            int32_t right = POP(sp);
            int32_t left = POP(sp);
            assert(right >= 0 && right < NUM_REGS);
            registers[right] = left;
            code++;
            break;
        }
        case '.': {
            POPS(stack, sp, 1);
            int32_t address = POP(sp);
            assert(address >= 0 && address < NUM_REGS);
            PUSH(sp, registers[address]);
            code++;
            break;
        }
        case '[':
            POPS(stack, sp, 1);
            if (!POP(sp)) {
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
            PUSHES(controlStack, cp, 1);
            PUSH(cp, code);
            break;
        case ')':
            POPS(controlStack, cp, 1);
            code = TOP(cp);
            break;
        case '^':
            POPS(stack, sp, 1);
            if (!POP(sp)) {
                eat(')');
                POPS(controlStack, cp, 1);
                POP(cp);
            } else {
                code++;
            }
            break;
        case '#': {
            code++;
            fp++;
            memset(fp, 0, sizeof(*fp));
            assert((*code >= 'a' && *code <= 'z') || (*code >= 'A' && *code <= 'Z'));
            char *macro = macros[tolower(*code) - 'a'];
            code++;
            // TODO(dannas): We need to handle recursive functions
            fp->pos_first_param = code;
            eat(';');
            fp->return_address = code;
            memcpy(fp->saved_regs, registers, NUM_REGS);
            code = macro;
            break;
        }
        case ',':
        case ';':
        case '@':
            memcpy(registers, fp->saved_regs, NUM_REGS);
            assert(fp >= callstack);
            code = fp->return_address;
            fp--;
            break;
        case '%': {
            code++;
            int32_t param_pos = POP(sp);
            char *param = fp->pos_first_param;
            int pos = 0;
            while (pos < param_pos) {
                assert(*param != '\0' && "scanned past eof");
                assert(*param != ';' && "pos out of range");
                if (*param == ',') {
                    pos++;
                }
                param++;
            }

            fp++;
            memset(fp, 0, sizeof(*fp));
            fp->return_address = code;
            memcpy(fp->saved_regs, registers, NUM_REGS);
            code = param;

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
                PUSHES(stack, sp, 1);
                PUSH(sp, *code);
                code++;
            }
            break;
        case '?':
            code++;
            PUSHES(stack, sp, 1);
            if (*code == '\'') {
                PUSH(sp, getchar());
                code++;
            } else  {
                int32_t val;
                scanf("%d", &val);
                PUSH(sp, val);
            }
            break;
        case '!':
            code++;
            POPS(stack, sp, 1);
            if (*code == '\'') {
                putchar(POP(sp));
                code++;
            } else {
                printf("%d", POP(sp));
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
                PUSH(sp, 1);
            }
            goto out;
            break;
        default:
            assert(0);
        }
    }
out:
    return POP(sp);
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
#if 0
    char program[] =
    "\" E: \"  #E,1000,#F,0;; \"!\"\n"
    "$E 1% x: x. 2% x. ! \"=\" ! @      ~ Scope of variables test\n"
    "$F 1% x: @                       ~ Should not change the x in $E\n";
    interpret(program);

#endif

    return 0;
}

