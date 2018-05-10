#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

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
        int32_t right = POP(); \
        int32_t left = POP(); \
        PUSHES(1); \
        PUSH(left op right); \
    } while (0)

typedef struct CallFrame {
    char *return_address;
    int32_t num_params;
    int32_t params[10];
} CallFrame;

CallFrame callstack[10];
CallFrame *fp =  NULL;

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
//  Should parameter indexes start from 0 or 1?

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

int32_t interpret(char *program) {
    enum {
        MAX_STACK = 32,
        NUM_REGS = 'Z'-'A' + 1,
        NUM_MACROS = NUM_REGS
    };

    int32_t stack[MAX_STACK];
    int32_t *top = stack;
    int32_t registers[NUM_REGS] = {0};

    char *macros[NUM_MACROS] = {NULL};

    fp = callstack;
    char* loop_header = NULL;

    code = program;

    while (*code) {
        if (*code == '$') {
            code++;
            if (isalpha(*code)) {
                char c = *code;
                code++;
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
            while (*code && *code != '\n') {
                code++;
            }
            break;
        case '0'...'9': {
            PUSHES(1);
            int32_t val = scan_int();
            PUSH(val);
            break;
        }
        case 'a'...'z':
        case 'A'...'Z':
            PUSHES(1);
            PUSH(tolower(*code) - 'a');
            code++;
            break;
        case ':': {
            POPS(2);
            int32_t right = POP();
            int32_t left = POP();
            assert(right >= 0 && right < NUM_REGS);
            registers[right] = left;
            code++;
            break;
        }
        case '.': {
            POPS(1);
            int32_t address = POP();
            assert(address >= 0 && address < NUM_REGS);
            PUSH(registers[address]);
            code++;
            break;
        }
        case '[':
            POPS(1);
            if (!POP()) {
                while (*code && *code != ']')
                    code++;
            } else {
                code++;
            }
            break;
        case ']':
            code++;
            break;
        case '(':
            loop_header = code;
            code++;
            break;
        case ')':
            code = loop_header;
            break;
        case '^':
            while (*code && *code != ')') {
                code++;
            }
            if (*code) {
                code++;
            }
            break;
        case '#':
            code++;
            assert(isalpha(*code));
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
            code = macros[tolower(name) - 'a'];

            break;
        case '@':
            assert(fp >= callstack);
            code = fp->return_address;
            fp--;
            break;
        case '%': {
            code++;
            int32_t index = scan_int();
            assert(index >= 0 && index < fp->num_params);
            PUSHES(1);
            PUSH(fp->params[index]);
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
            if (top == stack) {
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
    assert_interpret("(A.  1 +  A: A. 10 = [^]) A. ~ while (sum < 0) sum++;", 10);

    // Strings
    interpret("\"abc!def!\" 'x !' \"!\"$");

    // Macros
    assert_interpret("#a; $a 1 @", 1);
    assert_interpret("#a,2; $a %0 @", 2);

}

#undef assert_interpret

int main() {
    test_interpret();

    return 0;
}

