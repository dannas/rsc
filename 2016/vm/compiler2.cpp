// TODO(dannas): This is intended to be "basecompiler" that does lazy
// evaluation of stack values. It keeps values in registers until tnere are no
// more available register, at which point it spills.
//
// TODO(dannas): Add an Allocator class for keeping track of stack values.

#include "vm.h"
#include "codegenerator.h"

#include <cassert>
#include <map>
#include <set>

#define CASE break;case
#define UNKNOWN_OPCODE() assert(false && "unknown opcode")

using namespace std;
using BytecodePos = int32_t;

struct Offset {
    uint32_t offset;
};

struct Stk {
    enum Kind {
        REG,
        MEM,
        CONST
    };
    Kind kind;
    union {
        Reg r_;
        uint32_t offset_;
        Imm32 val_;
    };

    Stk(Reg r)      : kind(REG), r_(r) {}
    Stk(Offset o)   : kind(MEM), offset_(o.offset) {}
    Stk(Imm32 v)    : kind(CONST), val_(v) {}

    Reg reg()           { assert(kind == REG); return r_; }
    uint32_t offs()     { assert(kind == MEM); return offset_; }
    Imm32 val()         { assert(kind == CONST); return val_; }
};

CodeGenerator masm;


// I need a set of available registers
// I need a list of registers with values
// Maybe just use an int?
// I need to keep track of certain special registers on x64.
// So take a reg do the  operation and push the result
// If there's no registers available, then spill
// Spill also when we reach the end of a basic block, that would be
// a function call in this context.
// I need to be able to retrieve a spilled value. Is that done by first
// loading it into a set?
// TODO(dannas): Decide on which registers should be available
set<Reg> registerAvailable = {rbx, rcx, rsi, rdi};
vector<Stk> stk;

Reg allocReg() {
    assert(!registerAvailable.empty());
    // TODO(dannas): Surely there's a better way
    Reg r = *registerAvailable.begin();
    registerAvailable.erase(registerAvailable.begin());
    return r;
}

void freeReg(Reg r) {
    registerAvailable.insert(r);
}

Reg pop() {
    assert(!stk.empty());
    Stk& v = stk.back();
    Reg r;
    //assert(v.kind == Stk::REG);
    switch (v.kind) {
    case Stk::REG:
        r = v.reg();
        break;
    case Stk::MEM:
        assert(0);
        break;
    case Stk::CONST:
        r = allocReg();
        masm.mov(r, v.val());
        break;
    default:
        assert(0);
    }

    stk.pop_back();
    return r;
}

Reg pop(Reg specific) {
    assert(!stk.empty());
    // TODO(dannas): Assumes that specific is not in set of allocatable registers
    Reg r = pop();
    masm.mov(specific, r);
    freeReg(r);
    return specific;
}

void pushReg(Reg r) {
    stk.push_back(Stk(r));
}

void pushConst(Imm32 val) {
    stk.push_back(Stk(val));
}

static void emitPrologue(CodeGenerator& masm) {
    // The callee-saved registers on x64 are
    // r12-r15, rbx, rsp, rbp
    // We currently don't use registers beyond the first 8, so
    // ignore r12-r15.
    // We rely on the code being run with -fno-omit-framepointer
    // so rbp will be used as frame pointer inside and outside the jit.
    // TODO(dannas): Push parameters from rdi, rsi, rdx, rcx, r8, r9 to stack,
    // to allow passing in parameters to the JITted code.
    masm.push(rbp);
    masm.mov(rbp, rsp);
    masm.push(rbx);
}

static void emitEpilogue(CodeGenerator& masm) {
    masm.pop(rbx);
    masm.mov(rsp, rbp);
    masm.pop(rbp);
    masm.ret();
}

static int calcDisp(int nargs, int index) {
    // TODO(dannas): Replace '8' with constant for word length
    if (nargs > 0 && index < nargs)
        return (nargs + 1 - index) * 8;
    return -(1 + index - nargs) * 8;
}

MachineCode compile2(const Bytecode &code) {
    map<BytecodePos, Label> labels;
    map<BytecodePos, Label> functions;
    masm.buf().clear();

    emitPrologue(masm);

    size_t pos = 0;
    int nargs = 0;
    int nlocals = 0;

    while (pos < code.size()) {
        OpCode op = static_cast<OpCode>(code[pos]);
        Imm32 imm;
        BytecodePos addr;
        int index = 0;
        int disp;

        pos++; // Move to next opcode or operand

        switch (op) {
        CASE OP_POP:
            masm.pop(rax);
        CASE OP_IADD: {
            Reg rs = pop();
            Reg r = pop();
            masm.add(r, rs);
            freeReg(rs);
            pushReg(r);
        }
        CASE OP_ISUB: {
            Reg rs = pop();
            Reg r = pop();
            masm.sub(r, rs);
            freeReg(rs);
            pushReg(r);
        }
        CASE OP_IMULT: {
            Reg rs = pop();
            Reg r = pop();
            masm.mov(rax, r);
            masm.imul(rs);
            masm.mov(r, rax);
            freeReg(rs);
            pushReg(r);
        }
        CASE OP_IDIV: {
            Reg rs = pop();
            Reg r = pop();
            masm.mov(rax, r);
            masm.cqo();
            masm.idiv(rs);
            masm.mov(r, rax);
            freeReg(rs);
            pushReg(r);
        }
        CASE OP_IMOD: {
            Reg rs = pop();
            Reg r = pop();
            masm.mov(rax, r);
            masm.cqo();
            masm.idiv(rs);
            masm.mov(r, rdx);
            freeReg(rs);
            pushReg(r);
        }
        CASE OP_ICONST: {
            imm = code[pos++];
            pushConst(imm);
        }
        CASE OP_LABEL:
            masm.bind(labels[pos]);
        CASE OP_FUNC:
            // TODO(dannas): Why does jumps to OP_LABEL work, but
            // calls to OP_FUNC needs an adjustment of pos?
            masm.bind(functions[pos-1]);
            nargs = code[pos++];
            nlocals = code[pos++];
            masm.push(rbp);
            masm.mov(rbp, rsp);
            // TODO(dannas): Do we need to make sure stack is 16 bit aligned?
            masm.sub(rsp, (1 + nlocals) * 8); // TODO(dannas): Replace '8' with constant for word length
        CASE OP_ILT:
            masm.pop(rbx);
            masm.pop(rax);
            masm.cmpSet(LessThan, rax, rbx, rax);
            masm.push(rax);
        CASE OP_IEQ:
            masm.pop(rbx);
            masm.pop(rax);
            masm.cmpSet(Equal, rax, rbx, rax);
            masm.push(rax);
        CASE OP_BR:
            addr = code[pos++];
            masm.jump(labels[addr]);
        CASE OP_BRT:
            addr = code[pos++];
            masm.push(Imm32(1));
            masm.pop(rbx);
            masm.pop(rax);
            masm.cmp(rax, rbx);
            masm.j(AboveOrEqual, labels[addr]);
        CASE OP_LOAD: {
            index = code[pos++];
            disp = calcDisp(nargs, index);
            Reg r = allocReg();
            masm.mov(r, rbp, disp);
            pushReg(r);
        }
        CASE OP_STORE: {
            index = code[pos++];
            Reg r = pop();
            disp = calcDisp(nargs, index);
            masm.mov(rbp, disp, r);
            freeReg(r);
        }
        CASE OP_PRINT:
            UNKNOWN_OPCODE();
        CASE OP_CALL:
            addr = code[pos++];
            masm.call(functions[addr]);
            masm.push(rax);
        CASE OP_RET:
            masm.pop(rax);
            masm.mov(rsp, rbp);
            masm.pop(rbp);
            masm.ret();
        CASE OP_HALT: {
            pop(rax);
            emitEpilogue(masm);
            break;
        }
        default:
            UNKNOWN_OPCODE();
        }
    }

    return masm.buf();
}

