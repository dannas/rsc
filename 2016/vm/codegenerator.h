#pragma once

#include <vector>
#include <cassert>

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

class Label {
public:
    Label(uint32_t offset) : bound_(true), offset_(offset) {

    }
    Label() : bound_(false), offset_(INVALID_OFFSET) {
    }
    void use(uint32_t imm) {
        incomingEdges_.push_back(imm);
    }

    std::vector<uint32_t> incoming() {
        return incomingEdges_;
    }

    int32_t offset() {
        return offset_;
    }

    bool bound() {
        return bound_;
    }

    void bind(uint32_t offset) {
        offset_ = offset;
    }

    bool used() {
        return !bound() && offset_ < INVALID_OFFSET;
    }
    ~Label() {
        assert(!used());
    }

private:
    bool bound_;
    int32_t offset_;

    // TODO(dannas): Consider using the jump operands for encoding a jump list of
    // not yet resolved jumps as is done in the SpiderMonkey MacroAssemblers.
    std::vector<uint32_t> incomingEdges_;

    static const int32_t INVALID_OFFSET = INT32_MAX;
    friend bool operator==(const Label& lhs, const Label& rhs);
};

inline bool operator==(const Label& lhs, const Label& rhs) {
    return lhs.offset_ == rhs.offset_;
}

const uint8_t REX_W = 0x48;

class CodeGenerator {
public:
    std::vector<uint8_t> buf();
    uint8_t* data();
    size_t size();

    // TODO(dannas): Add a method for fetching Labels.
    // For a bytecode with structured control flow there would
    // be no need for keeping track of the labels since each block
    // (iteration or selection) would have a predefined limited scope.

    // MacroAssembler methods
    void jump(Label& label);
    void bind(Label& label);
    void patch(size_t src, uint32_t dst);

    // Codegenerator methods
    void add(Reg dst, Reg src);
    void cqo();
    void idiv(Reg src);
    void imul(Reg src);
    void int3();
    void jmp(Imm32 imm);
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
    std::vector<Label> labels_;
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

inline void CodeGenerator::jump(Label& label) {
    // If the label is not bound, then record the jmp src for later patching.
    if (!label.bound()) {
        label.use(size());
        jmp(Imm32(0));
    } else {
        jmp(label.offset());
    }
}

inline void CodeGenerator::bind(Label& label) {
    // TODO(dannas): There must always be one more instruction after a label. How
    // enforce that?
    label.bind(size());

    uint32_t dst = label.offset();

    if (label.used()) {
        // Traverse the list of incoming jmp sources and patch their locations.
        for (uint32_t src : label.incoming()) {
            // TODO(dannas): Ensure that |src| points to jmp operand.
            patch(src, dst);
        }
    }
}

inline void CodeGenerator::patch(size_t src, uint32_t dst) {
    buf_[src] =   (dst      ) & 0xff;
    buf_[src+1] = (dst >> 8 ) & 0xff;
    buf_[src+2] = (dst >> 16) & 0xff;
    buf_[src+3] = (dst >> 24) & 0xff;
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

inline void CodeGenerator::int3() {
    emit(0xcc);
}

inline void CodeGenerator::jmp(Imm32 imm) {
    emit(0xe9);
    emit(imm.bytes[0]);
    emit(imm.bytes[1]);
    emit(imm.bytes[2]);
    emit(imm.bytes[3]);
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

