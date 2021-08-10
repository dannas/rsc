// Interpreter for the Mouse language.
// http://mouse.davidgsimpson.com/mouse83/intro83.html
// http://mouse.davidgsimpson.com/mouse83/ref83.html
// http://www.retroprogramming.com/2012/08/mouse-language-for-microcomputers-by.html
//
// TODO
//      Decide if we should have bounds checks or just rely on $ for end of program
//      Add error reporting
//      Fix recursive calls
//      Move regs from stack
//      Let loops use envstack
//      Comment the code
//      Write debugger
//      Add documentation of language like http://www.retroprogramming.com/2012/08/mouse-language-for-microcomputers-by.html

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
};

typedef enum FrameType {
    CONTROL_FRAME,
    PARAMETER_FRAME,
    MACRO_FRAME
} FrameType;

typedef struct CallFrame {
    FrameType type;
    char *old_code;
    int32_t saved_regs[NUM_REGS];
} CallFrame;

char *code = NULL;

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
    assert(code);

    while (true) {
        assert(code);
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

int print_code(char *code_str) {
    if (!code_str) {
        printf("     ");
        return 8;
    }
    for (int i = 0; i < 8; ) {
        if (isspace(*code_str)) {
            putchar(' ');
            code_str++;
            i++;
        } else if (*code_str == '\0') {
            putchar(' ');
            i++;
        } else {
            putchar(*code_str);
            code_str++;
            i++;
        }
    }
    return 8;
}

int32_t interpret(char *program) {
    int32_t stack[MAX_STACK];
    int32_t *sp = stack;

    char *controlStack[MAX_STACK];
    char **cp = controlStack;

    CallFrame callstack[MAX_STACK];
    CallFrame *fp = callstack;

    int32_t register_banks[NUM_REGS * MAX_STACK] = {0};
    int32_t *registers = register_banks;
    char *macros[NUM_MACROS] = {NULL};

    bool tracing = false;

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
        if (tracing) {
            int n = 0;
            printf("%ld    ", (fp-callstack));
            n += print_code(code);
            n += printf("    REGS: ");
            for (int i = 0; i < NUM_REGS; ++i) {
                n += printf("%c=%d ", 'A' + i, registers[i]);
            }
            n += printf("    STACK: ");
            for (int *p = stack; p < sp; ++p) {
                n += printf("%d ", *p);
            }
            printf("%*s", 165 - n, "CALLS: ");
            for (CallFrame *f = callstack; f <= fp; ++f) {
                print_code(f->old_code);
                printf("    ");
            }
            printf("\n");
            fflush(stdout);
        }

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
                (void)POP(cp);
            } else {
                code++;
            }
            break;
        case '#': {
            code++;
            fp++;
            memset(fp, 0, sizeof(*fp));
            fp->type = MACRO_FRAME;
            assert((*code >= 'a' && *code <= 'z') || (*code >= 'A' && *code <= 'Z'));
            char *macro = macros[tolower(*code) - 'a'];
            code++;
            fp->old_code = code;

            while (*code != ';') {
                code = next_param();
            }
            assert(*code == ';');
            code++;

            registers += NUM_REGS;
            //memcpy(fp->saved_regs, registers, NUM_REGS * sizeof(registers[0]));
            code = macro;
            break;
        }
        case '@':
            registers -= NUM_REGS;
            //memcpy(registers, fp->saved_regs, NUM_REGS * sizeof(registers[0]));
            assert(fp >= callstack);
            code = fp->old_code;

            while (*code != ';') {
                code = next_param();
            }
            assert(*code == ';');
            code++;
            fp--;
            break;
        case '%': {
            code++;
            int32_t param_pos = POP(sp);
            char *old_code = code;
            code = fp->old_code;
            fp++;
            memset(fp, 0, sizeof(*fp));
            fp->type = PARAMETER_FRAME;
            fp->old_code = old_code;

            // TODO(dannas):
            // Find topmost macro frame
            // Set code to pos of that frame (start of macro-call)
            // Find current parameter
            assert (fp >= callstack);

            for (int pos = 0; pos < param_pos; pos++) {
                code = next_param();
            }
            break;
        }
        case ',':
        case ';':
            assert(fp >= callstack);
            code = fp->old_code;
            fp--;
            break;
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
        case '{':
            tracing = true;
            code++;
            break;
        case '}':
            tracing = false;
            code++;
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
    assert_interpret("#D,#D,1,2;,#D,3,4;; $D 1% 2% + @", 10);
}

#undef assert_interpret

char *path_file(char path[PATH_MAX]) {
    for (char *ptr = path + strlen(path); ptr != path; ptr--) {
        if (ptr[-1] == '/') {
            return ptr;
        }
    }
    return path;
}

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

int main(int argc, char *argv[]) {
    test_interpret();

    if (argc != 2) {
        printf("Usage: %s mouse-file\n", path_file(argv[0]));
        return 1;
    }

    char *program = read_file(argv[1]);
    if (!program) {
        perror("read_file");
        exit(1);
    }

    interpret(program);
    return 0;
}

