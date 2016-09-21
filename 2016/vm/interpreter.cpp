#include <iostream>
#include <cstdint>
#include <cassert>
#include <vector>

#include <gtest/gtest.h>

#include "opcodes.h"

#define CASE break;case

using namespace std;

// NAME
//      stackvm4 - minimal stackbased bytecode interpreter
//
// SYNOPSIS
//      TODO
//
// DESCRIPTION
//      TODO
//
// LIMITATIONS
//      TODO

const int kMaxStackSize = 64;	// TODO(dannas): Max stack size is hardcoded

class Stack {
public:
    Stack() : sp_(0), fp_(0)  {
    }

    void push(int32_t val) {
        checkRep();
        arr_[sp_++] = val;
    }
    void pushfp() {
        int oldfp = fp_;
        fp_ = sp_;
        arr_[sp_++] = oldfp;
    }

    void popfp() {
        fp_ = pop();
    }

    int32_t pop() {
        checkRep();
        return arr_[--sp_];
    }

    int32_t top() {
        checkRep();
        return arr_[sp_ - 1];
    }

    void load(int offset) {
        checkRep();
        push(arr_[fp_+offset]);
    }

    void store(int offset) {
        checkRep();
        arr_[fp_+offset] = pop();
    }

private:
    void checkRep() {
        assert(sp_ >= 0 && sp_ < kMaxStackSize && fp_ >= -1 && fp_ <= sp_);
    }
    friend ostream& operator<< (ostream& os, const Stack& stack);

    int32_t arr_[kMaxStackSize];	// the stack
    int sp_;					    // stack pointer
    int fp_;						// frame pointer
};

ostream& operator<< (ostream& os, const Stack& stack) {
    os << "[";

    if (stack.sp_ > 0)
        os << stack.arr_[0];

    for (int i = 1; i < stack.sp_; i++)
        os << ", " << stack.arr_[i];

    os << "]";
    return os;
}

// Interpret the bytecode in |code| using global variables stored in |globals|.
void interpret(int32_t* code, int32_t* globals, ostream& out = cout) {

    Stack stack;

    int ip = 0;

    while (true) {
        OpCode op = static_cast<OpCode>(code[ip]);

        cout << op << "\t" << stack << "\n";

        ip++; // Move to next opcode or operand

        switch (op) {
            int x, y, addr, nargs, ret;
        CASE OP_IADD:
            x = stack.pop();
            y = stack.pop();
            stack.push(y + x);
        CASE OP_ISUB:
            x = stack.pop();
            y = stack.pop();
            stack.push(y - x);
        CASE OP_ICONST:
            x = code[ip++];
            stack.push(x);
        CASE OP_BRT:
            addr = code[ip++];
            if (stack.top())
                ip = addr;
        CASE OP_GLOAD:
            x = code[ip++];
            y = globals[x];
            stack.push(y);
        CASE OP_GSTORE:
            x = code[ip++];
            y = stack.pop();
            globals[x] = y;
        CASE OP_LOAD:
            x = code[ip++];
            stack.load(x);
        CASE OP_STORE:
            x = code[ip++];
            stack.store(x);
        CASE OP_PRINT:
            out << stack.pop() << "\n";
        CASE OP_CALL:
            addr = code[ip++];
            nargs = code[ip++];
            stack.push(nargs);
            stack.pushfp();
            stack.push(ip);
            ip = addr;
        CASE OP_RET:
            ret = stack.pop();
            ip = stack.pop();
            stack.popfp();
            nargs = stack.pop();
            while (nargs--)
                stack.pop();
            stack.push(ret);
        CASE OP_HALT:
            return;
        default:
            assert(false && "unknown opcode");
        }
    }
}

#define INTERPRET_AND_COMPARE(code, expected) \
    do { \
        int32_t globals[] = {}; \
        stringstream ss; \
        interpret(code, globals, ss); \
        ss.seekg(0); \
        string actual = ss.str(); \
        ASSERT_EQ(actual, expected); \
    } while (0)


TEST(Interpreter, halt) {
    int32_t code[] = { OP_HALT };
    string expected = "";

    INTERPRET_AND_COMPARE(code, expected);
}

TEST(Interpreter, add) {
    int32_t code[] = {
        OP_ICONST, 1,
        OP_ICONST, 1,
        OP_IADD,
        OP_PRINT,
        OP_HALT
    };
    string expected = "2\n";

    INTERPRET_AND_COMPARE(code, expected);
}

TEST(Interpreter, callZeroParams) {
    int32_t code[] = {
        OP_CALL, 5, 0,
        OP_PRINT,
        OP_HALT,
        OP_ICONST, 42,
        OP_RET
    };
    string expected = "42\n";

    INTERPRET_AND_COMPARE(code, expected);
}

TEST(Interpreter, callOneParam) {
    int32_t code[] = {
        OP_ICONST, 1,
        OP_CALL, 7, 1,
        OP_PRINT,
        OP_HALT,
        OP_LOAD, -1,
        OP_RET
    };
    string expected = "1\n";

    INTERPRET_AND_COMPARE(code, expected);
}

TEST(Interpreter, callTwoParams) {
    int32_t code[] = {
        OP_ICONST, 1,
        OP_ICONST, 2,
        OP_CALL, 9, 2,
        OP_PRINT,
        OP_HALT,
        OP_LOAD, -3,
        OP_LOAD, -2,
        OP_IADD,
        OP_RET
    };
    string expected = "3\n";

    INTERPRET_AND_COMPARE(code, expected);
}

string progname(char* s) {
    char* p = strrchr(s, '/');
    if (p == nullptr)
        p = s;
    else
        p++;
    return string(p);
}
int main(int argc, char* argv[]) {

    string pn = progname(argv[0]);

    if (pn == "interpreter-test") {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }

    // TODO(dannas): globals is hardcoded.
    int32_t globals[] = {};

    if (argc != 2) {
        cerr << "usage: " << argv[0] << " FILE\n";
        exit(1);
    }
    vector<int32_t> code;
    FILE* fp = fopen(argv[1], "r");
    assert(fp);

    while (!feof(fp)) {
        int32_t d;
        fread(&d, 1, sizeof(d), fp);
        if (ferror(fp)) {
            perror("fread");
            exit(1);
        }
        code.push_back(d);
    }
    interpret(code.data(), globals);
}
