#include "vm.h"
#include "codegenerator.h"

#include <cassert>
#include <map>

#define CASE break;case
#define UNKNOWN_OPCODE() assert(false && "unknown opcode")

using namespace std;
using BytecodePos = int32_t;

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

MachineCode compile(const Bytecode &code) {
    CodeGenerator masm;
    map<BytecodePos, Label> labels;
    map<BytecodePos, Label> functions;

    emitPrologue(masm);

    size_t pos = 0;
    int nargs = 0;
    int nlocals = 0;

    while (pos < code.size()) {
        OpCode op = static_cast<OpCode>(code[pos]);
        Imm32 imm;
        BytecodePos addr;
        int index = 0;

        pos++; // Move to next opcode or operand

        switch (op) {
        CASE OP_POP:
            UNKNOWN_OPCODE();
        CASE OP_IADD:
            masm.pop(rax);
            masm.pop(rbx);
            masm.add(rax, rbx);
            masm.push(rax);
        CASE OP_ISUB:
            masm.pop(rbx);
            masm.pop(rax);
            masm.sub(rax, rbx);
            masm.push(rax);
        CASE OP_IMULT:
            masm.pop(rbx);
            masm.pop(rax);
            masm.imul(rbx);
            masm.push(rax);
        CASE OP_IDIV:
            masm.pop(rbx);
            masm.pop(rax);
            masm.cqo();
            masm.idiv(rbx);
            masm.push(rax);
        CASE OP_IMOD:
            masm.pop(rbx);
            masm.pop(rax);
            masm.cqo();
            masm.idiv(rbx);
            masm.push(rdx);
        CASE OP_ICONST:
            imm = code[pos++];
            masm.push(imm);
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
        CASE OP_LOAD:
            index = code[pos++];
            if (nargs > 0 && index < nargs) { 
                // TODO(dannas): Replace '8' with constant for word length
                int disp = (nargs + 1 - index) * 8;
                masm.mov(rax, rbp, disp);
            } else {
                int disp = -(1 + index - nargs) * 8;
                masm.mov(rax, rbp, disp);
            }
            masm.push(rax);
        CASE OP_STORE:
            index = code[pos++];
            masm.pop(rax);
            if (nargs > 0 && index < nargs) { 
                // TODO(dannas): Replace '8' with constant for word length
                int disp = (nargs + 1 - index) * 8;
                masm.mov(rbp, disp, rax);
            } else {
                int disp = -(1 + index - nargs) * 8;
                masm.mov(rbp, disp, rax);
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
        CASE OP_HALT:
            // TODO(dannas): Consider adding a jmp here for another location
            // where emitEpilogue is placed.
            masm.pop(rax);
            emitEpilogue(masm);
            break;
        default:
            UNKNOWN_OPCODE();
        }
    }

    return masm.buf();
}

