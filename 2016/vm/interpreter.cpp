#include <iostream>
#include <cstdint>
#include <cassert>
#include <vector>

#include "vm.h"

#define CASE break;case
#define UNKNOWN_OPCODE() assert(false && "unknown opcode")

using namespace std;

const int kMaxStackSize = 64;	// TODO(dannas): Max stack size is hardcoded

// This class represents the operand stack.
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
        push(oldfp);
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

    void load(int index) {
        checkRep();
        // TODO(dannas): The stack should look like this:
        // parameters
        // ret
        // locals
        // nargs+nlocals
        // oldfp  <=== fp
        int nlocals = arr_[fp_-1];
        if (index < 0) {
            int offset = fp_ - 1 - nlocals - 1 + index;
            push(arr_[offset]);
        } else {
            int offset = fp_ - 1 - 1 - nlocals + index;
            push(arr_[offset]);
        }
    }

    void store(int index) {
        checkRep();
        int nlocals = arr_[fp_-2];
        int offset = index - nlocals - 1;
        arr_[fp_+offset] = pop();
    }

private:
    void checkRep() {
        assert(sp_ >= 0);
        assert(sp_ < kMaxStackSize);
        assert(fp_ >= -1);
        assert(fp_ <= sp_);
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

    for (int i = 1; i < stack.sp_; i++) {
        os << ", " << stack.arr_[i];
        if (i ==  stack.fp_)
            os << "*";
    }

    os << "]";
    return os;
}

void interpret(const Bytecode &code, ostream& out) {

    Stack stack;

    int ip = 0;

    while (true) {
        OpCode op = static_cast<OpCode>(code[ip]);

        //cout << ip << " " << op << "\t" << stack << "\n";

        ip++; // Move to next opcode or operand

        switch (op) {
            int x, y, addr, nargs, nlocals, ret;
        CASE OP_POP:
            stack.pop();
        CASE OP_IADD:
            x = stack.pop();
            y = stack.pop();
            stack.push(y + x);
        CASE OP_ISUB:
            x = stack.pop();
            y = stack.pop();
            stack.push(y - x);
        CASE OP_IMULT:
            x = stack.pop();
            y = stack.pop();
            stack.push(y * x);
        CASE OP_IDIV:
            x = stack.pop();
            y = stack.pop();
            stack.push(y / x);
        CASE OP_IMOD:
            x = stack.pop();
            y = stack.pop();
            stack.push(y % x);
        CASE OP_ICONST:
            x = code[ip++];
            stack.push(x);
        CASE OP_ILT:
            x = stack.pop();
            y = stack.pop();
            stack.push(y < x);
        CASE OP_IEQ:
            x = stack.pop();
            y = stack.pop();
            stack.push(y == x);
        CASE OP_LABEL:
            ;
        CASE OP_FUNC:
            nargs = code[ip++];
            nlocals = code[ip++];
            for (int i = 0; i < nlocals; i++)
                stack.push(0);
            stack.push(nlocals);
            stack.pushfp();
        CASE OP_BR:
            addr = code[ip++];
            ip = addr;
        CASE OP_BRT:
            addr = code[ip++];
            if (stack.top())
                ip = addr;
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
            stack.push(ip);
            ip = addr;
        CASE OP_RET:
            ret = stack.pop();
            stack.popfp();
            nlocals = stack.pop();
            while (nlocals--)
                stack.pop();
            ip = stack.pop();
            stack.push(ret);
        CASE OP_HALT:
            return;
        default:
            UNKNOWN_OPCODE();
        }
    }
}
