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

    while (pos < code.size()) {
        OpCode op = static_cast<OpCode>(code[pos]);
        Imm32 imm;
        BytecodePos addr;
        int nargs, nlocals;

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
            masm.sub(rsp, nlocals * sizeof(int32_t));
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
            // TODO(dannas): Should we use a caller or callee cleanup calling convention?
            //
            // CDECL is caller cleanup.
            // The caller pushes the arguments and later pops them.
            //
            // The Pascal/stdcall conventions are callee cleanup.
            // The funtions knows how many bytes have been pushes as arguments.
            // The ret instruction has an optional 16 bit operand that specifies how many
            // bytes shall be popped off the stack when returning.
            //
            // A third option is to push nargs onto the stack. But then, we'll have to calculate
            // the load index at runtime in assembly. Fiddly!
            //
            // Should I add a stack ADT for keepinng track of number of arguments?
            // If I were to use a stack then I run into trouble for caller
            // cleanup calling conventions. At the moment, we don't know how
            // many ops a function has at the OP_CALL instruction. We need that
            // there for popping the stack.
            //
            // Maybe I should use a use a calle-cleanup convention and keep track of number of
            // arguments on the stack via a stack ADT. By the time I
            // reach OP_RET, I can pop the stack and ret <NARGS>. That requires
            // that there's only one OP_RET for each function though. Maybe the bytecode
            // should be canonicalized to ensure there' only one exit point for
            // each function.
            //
            // A OP_FUNC_END annotation would allow us to keep track of the proper stack level as well

            // ### mov rax, [rbp+index]
            // ### push rax
            UNKNOWN_OPCODE();
        CASE OP_STORE:
            // ### pop rax
            // ### mov [rbp+index], rax
            UNKNOWN_OPCODE();
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

