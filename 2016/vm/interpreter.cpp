#include <iostream>
#include <cstdint>
#include <cassert>
#include <vector>

#include "opcodes.h"

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
void interpret(int32_t* code, int32_t* globals) {

    Stack stack;

    int ip = 0;

    while (true) {
        OpCode op = static_cast<OpCode>(code[ip]);

        cout << op << "\t" << stack << "\n";

        ip++; // Move to next opcode or operand

        switch (op) {
            int x, y, addr, nargs, ret;
        case OP_IADD:
            x = stack.pop();
            y = stack.pop();
            stack.push(y + x);
            break;
        case OP_ISUB:
            x = stack.pop();
            y = stack.pop();
            stack.push(y - x);
            break;
        case OP_ICONST:
            x = code[ip++];
            stack.push(x);
            break;
        case OP_BRT:
            addr = code[ip++];
            if (stack.top())
                ip = addr;
            break;
        case OP_GLOAD:
            x = code[ip++];
            y = globals[x];
            stack.push(y);
            break;
        case OP_GSTORE:
            x = code[ip++];
            y = stack.pop();
            globals[x] = y;
            break;
        case OP_LOAD:
            x = code[ip++];
            stack.load(x);
            break;
        case OP_STORE:
            x = code[ip++];
            stack.store(x);
            break;
        case OP_PRINT:
            cout << stack.pop() << "\n";
            break;
        case OP_CALL:
            addr = code[ip++];
            nargs = code[ip++];
            stack.push(nargs);
            stack.pushfp();
            stack.push(ip);
            ip = addr;
            break;
        case OP_RET:
            ret = stack.pop();
            ip = stack.pop();
            stack.popfp();
            nargs = stack.pop();
            while (nargs--)
                stack.pop();
            stack.push(ret);
            break;
        case OP_HALT:
            return;
        default:
            assert(false && "unknown opcode");
        }
    }
}

int main(int argc, char* argv[]) {

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
