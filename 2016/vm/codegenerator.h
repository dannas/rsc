#pragma once

#include <vector>

// Encoding x86 instructions
// http://www.c-jump.com/CIS77/CPU/x86/lecture.html
// http://wiki.osdev.org/X86-64_Instruction_Encoding

enum Reg : uint8_t{
    RAX = 0,
    RCX = 1,
    RDX = 2,
    RBX = 3,
    RSP = 4,
    RBP = 5,
    RSI = 6,
    RDI = 7
};

enum Mod : uint8_t {
    INDIRECT = 0,
    INDIRECT_DISP1 = 1,
    INDIRECT_DISP4 = 2,
    DIRECT = 3
};

struct Imm32 {
    Imm32(uint32_t v) : val(v) {}
    union {
        uint32_t val;
        uint8_t bytes[4];
    };
};

const uint8_t REX_W = 0x48;

class CodeGenerator {
public:
    std::vector<uint8_t> buf();
    uint8_t* data();
    size_t size();

    void add(Reg dst, Reg src);
    void cqo();
    void idiv(Reg src);
    void imul(Reg src);
    void mov(Reg dst, Reg src);
    void pop(Reg dst);
    void push(Reg src);
    void push(Imm32 imm);
    void ret();
    void sub(Reg dst, Reg src);

private:
    void emit(uint8_t b);
    void emitModRM(uint8_t mod, uint8_t r, uint8_t m);

    std::vector<uint8_t> buf_;
};

inline std::vector<uint8_t> CodeGenerator::buf() {
    return buf_;
}

inline uint8_t* CodeGenerator::data() {
    return buf_.data();
}

inline size_t CodeGenerator::size() {
    return buf_.size();
}

inline void CodeGenerator::add(Reg dst, Reg src) {
    emit(REX_W);
    emit(0x01);
    emitModRM(DIRECT, src, dst);
}

inline void CodeGenerator::cqo() {
    emit(REX_W);
    emit(0x99);
}

inline void CodeGenerator::sub(Reg dst, Reg src) {
    emit(REX_W);
    emit(0x29);
    emitModRM(DIRECT, src, dst);
}

inline void CodeGenerator::imul(Reg src) {
    emit(REX_W);
    emit(0xf7);
    emitModRM(DIRECT, 5, src);
}

inline void CodeGenerator::idiv(Reg src) {
    emit(REX_W);
    emit(0xf7);
    emitModRM(DIRECT, 7, src);
}

inline void CodeGenerator::ret() {
    emit(0xc3);
}

inline void CodeGenerator::mov(Reg dst, Reg src) {
    emit(REX_W);
    emit(0x89);
    emitModRM(DIRECT, src, dst);
}

inline void CodeGenerator::push(Reg src) {
    emit(0x50 | src);
}

inline void CodeGenerator::push(Imm32 imm) {
    emit(0x68);
    emit(imm.bytes[0]);
    emit(imm.bytes[1]);
    emit(imm.bytes[2]);
    emit(imm.bytes[3]);
}

inline void CodeGenerator::pop(Reg dst) {
    emit(0x58 | dst);
}

inline void CodeGenerator::emit(uint8_t b) {
    buf_.push_back(b);
}

inline void CodeGenerator::emitModRM(uint8_t mod, uint8_t r, uint8_t m) {
    emit(mod << 6 | r << 3 | m);
}
