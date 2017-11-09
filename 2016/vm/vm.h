#pragma once

#include <iosfwd>
#include <cstdint>
#include <vector>

// GRAMMAR ===
//
// program              => functiondeclaration*
// functiondeclaration  => NEWLINE* '.def' ID 'args' '=' OPERAND ',' 'locals' '=' OPERAND NEWLINE instr*
// labeldeclaration     => NEWLINE* LABEL
// instr                => ID NEWLINE
//                      => ID OPERAND NEWLINE
//                      => ID LABEL NEWLINE
//                      => ID OPERAND ',' OPERAND NEWLINE
// ENVIRONMENT ===
//
// One stack is used both as operand stack and for keeping track of stack frames.
// Two of the machines registers points into the stack (which grows upwards):
//
//  SP points to the top of the stack (the stack pointer)
//  FP marks the beginning of the active stack frame (the frame pointer)
//
// CALLING CONVENTION ===
//
// This is the layout of a stack frame:
//
//  SP =>
//         local stack
//         locals
//         parameters
//         return address
//  FP =>  previous frame pointer
//         nargs
//

// Assemble the assembly code in the stream provided by |in| into bytecode.
//
std::vector<int32_t> assemble(std::istream& in);

// Interpret the bytecode in |code|.
//
// |code| must be well formed bytecode - as produced by the assembler step.
// The executing bytecode can interact with the outside by printing stdout
// to |out|.
void interpret(const std::vector<int32_t> &code, std::ostream& out);

// Disassemble the bytecode pointed to by |in|.
//
// Return a string containing the disassembled bytecode.
std::string disassemble(std::istream& in);

// Compile the bytecode in |code|.
//
// |code| must be well formed bytecode - as produced by the assembler step.
std::vector<uint8_t> compile(const std::vector<int32_t>& code);

// PRIVATE INTERFACE

// List of available bytecodes on the form
//  enum-value   string-name   num-arguments
#define FOR_EACH_OPCODE(macro)      \
    \
    /* Pops the topmost value from the stack.
     * Stack: val =>
     */ \
    macro(OP_POP,    "pop",     0)  \
    \
    /* Pops the two values 'lval' and 'rval' from the stack, then pushes
     * the result of applying the arithmetic operation to them.
     * Stack: lval, rval => (lval OP rval)
     */ \
    macro(OP_IADD,    "iadd",   0)  \
    macro(OP_ISUB,    "isub",   0)  \
    macro(OP_IMULT,   "imult",  0)  \
    macro(OP_IDIV,    "idiv",   0)  \
    macro(OP_IMOD,    "imod",   0)  \
    \
    /* Pushes a constant onto the stack.
     * Stack: => constant
     */ \
    macro(OP_ICONST,  "iconst", 1)  \
    \
    /* Pops the top two values from the stack and pushes the result of
     * comparing them.
     * Stack: lval, rval => (lval OP rval)
     */ \
    macro(OP_ILT,     "ilt",    0)  \
    macro(OP_IEQ,     "ieq",    0)  \
    \
    /* NOP used while emitting code.
     *
     */ \
    macro(OP_LABEL,   "label",  0) \
    \
    /* Jumps to an absolute address unconditionally.
     * Stack: =>
     */ \
    macro(OP_BR,      "br",     1)  \
    \
    /* Jumps to an absolute address if top value on stack evaluates to true.
     * Operands: int32_t absolute address
     * Stack: =>
     */ \
    macro(OP_BRT,     "brt",    1)  \
    \
    /* Pushes the parameter 'param' onto the stack.
     * Operands: int32_t index into parameters further down the stack.
     * Stack: param1, param2, ..., val => param1, param2, ..., val, param1
     */ \
    macro(OP_LOAD,    "load",   1)  \
    \
    /* Pops the topmost stack value and stores it in the parameter stack area.
     * Operands: int32_t index into parameters further down the stack.
     * Stack: param1, param2, ..., v1, v2 => v2, param2, ..., v1
     */ \
    macro(OP_STORE,   "store",  1)  \
    \
    /* Pops the top value from the stack and prints it to stdout.
     * Stack: v1 =>
     */ \
    macro(OP_PRINT,   "print",  0)  \
    \
    /* Invokes 'callee' with arguments previously pushed onto the stack. See
     * Calling convention paragraph at top of this file.
     * Stack: TODO
     */ \
    macro(OP_CALL,    "call",   3)  \
    \
    /* Pops the topmost val of the stack, then pops 'locals', 'parameters',
     * 'nargs', 'fp' and 'retaddr' before pushing the stored val. Sets the ip
     * to 'retaddr'
     * Stack: nargs, retaddr, fp, parameters, locals, val => val
     */ \
    macro(OP_RET,     "ret",    0)  \
    \
    /* Stops the execution of the script.
     * Stack: =>
     */ \
    macro(OP_HALT,    "halt",   0)  \

enum OpCode : int32_t {
#define macro(op, desc, nargs) op,
    FOR_EACH_OPCODE(macro)
#undef macro
};

bool instrExists(const std::string& instr);

OpCode opCodeForInstr(const std::string& instr);

std::ostream& operator<< (std::ostream& os, OpCode code);
