#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
    char *macro;
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
        case '#':
            code++;
            fp++;
            memset(fp, 0, sizeof(*fp));
            assert((*code >= 'a' && *code <= 'z') || (*code >= 'A' && *code <= 'Z'));
            fp->macro = macros[tolower(*code) - 'a'];
            // TODO(dannas): Cleanup. Dummy value for making ';' and ',' cases consistent: They expect a value to exist on the stack
            // that should be used as parameter. Let's have param[0] be a dummy.
            PUSH(sp, 0);
            code++;
            break;
        case ',':
            fp->params[fp->num_params] = POP(sp);
            fp->num_params++;
            code++;
            break;
        case ';':
            fp->params[fp->num_params] = POP(sp);
            fp->num_params++;
            code++;
            fp->return_address = code;
            memcpy(fp->saved_regs, registers, NUM_REGS);
            code = fp->macro;
            break;
        case '@':
            memcpy(registers, fp->saved_regs, NUM_REGS);
            assert(fp >= callstack);
            code = fp->return_address;
            fp--;
            break;
        case '%': {
            code++;
            int32_t index = POP(sp);
            assert(index > 0 && index <= fp->num_params);
            PUSH(sp, fp->params[index]);
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
#if 0
    test_interpret();

    char *program = read_file("../test.mouse");
    if (!program) {
        perror("read_file");
        exit(1);
    }

    interpret(program);
#endif
    char program[] =
    "\" E: \"  #E,1000,#F,0;; \"!\"\n"
    "$E 1% x: x. 2% x. ! \"=\" ! @      ~ Scope of variables test\n"
    "$F 1% x: @                       ~ Should not change the x in $E\n";
    interpret(program);


    return 0;
}

