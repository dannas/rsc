#pragma once

#include <iosfwd>
#include <cstdint>
#include <vector>

// GRAMMAR ===
//
// program              => globaldeclaration functiondeclaration*
// globalsdeclaration   => NEWLINE* '.globals'
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
// TODO(dannas): What about a constant pool?
// TODO(dannas): What about memory?
//
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

// Interpret the bytecode in |code| using global variables stored in |globals|.
//
// |code| must be well formed bytecode - as produced by the assembler step.
// The executing bytecode can interact with the outside by printing stdout
// to |out|.
void interpret(int32_t* code, int32_t* globals, std::ostream& out);

// PRIVATE INTERFACE

// List of available bytecodes on the form
//  enum-value   string-name   num-arguments
#define FOR_EACH_OPCODE(macro)      \
    macro(OP_IADD,    "iadd",   0)  \
    macro(OP_ISUB,    "isub",   0)  \
    macro(OP_ICONST,  "iconst", 1)  \
    macro(OP_ILT,     "ilt",    0)  \
    macro(OP_BRT,     "brt",    1)  \
    macro(OP_GLOAD,   "gload",  1)  \
    macro(OP_GSTORE,  "gstore", 1)  \
    macro(OP_LOAD,    "load",   1)  \
    macro(OP_STORE,   "store",  1)  \
    macro(OP_PRINT,   "print",  0)  \
    macro(OP_CALL,    "call",   1)  \
    macro(OP_RET,     "ret",    0)  \
    macro(OP_HALT,    "halt",   0)  \

enum OpCode : int32_t {
#define macro(op, desc, nargs) op,
    FOR_EACH_OPCODE(macro)
#undef macro
};

bool instrExists(const std::string& instr);

OpCode opCodeForInstr(const std::string& instr);

std::ostream& operator<< (std::ostream& os, OpCode code);
