#include <cstdint>
#include <sys/mman.h>
#include <cstring>
#include <cstdio>
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

uint8_t* CodeGenerator::data() {
    return buf_.data();
}

size_t CodeGenerator::size() {
    return buf_.size();
}

void CodeGenerator::add(Reg dst, Reg src) {
    emit(REX_W);
    emit(0x01);
    emitModRM(DIRECT, src, dst);
}

void CodeGenerator::cqo() {
    emit(REX_W);
    emit(0x99);
}

void CodeGenerator::sub(Reg dst, Reg src) {
    emit(REX_W);
    emit(0x29);
    emitModRM(DIRECT, src, dst);
}

void CodeGenerator::imul(Reg src) {
    emit(REX_W);
    emit(0xf7);
    emitModRM(DIRECT, 5, src);
}

void CodeGenerator::idiv(Reg src) {
    emit(REX_W);
    emit(0xf7);
    emitModRM(DIRECT, 7, src);
}

void CodeGenerator::ret() {
    emit(0xc3);
}

void CodeGenerator::mov(Reg dst, Reg src) {
    emit(REX_W);
    emit(0x89);
    emitModRM(DIRECT, src, dst);
}

void CodeGenerator::push(Reg src) {
    emit(0x50 | src);
}

void CodeGenerator::push(Imm32 imm) {
    emit(0x68);
    emit(imm.bytes[0]);
    emit(imm.bytes[1]);
    emit(imm.bytes[2]);
    emit(imm.bytes[3]);
}

void CodeGenerator::pop(Reg dst) {
    emit(0x58 | dst);
}

void CodeGenerator::emit(uint8_t b) {
    buf_.push_back(b);
}

void CodeGenerator::emitModRM(uint8_t mod, uint8_t r, uint8_t m) {
    emit(mod << 6 | r << 3 | m);
}

int main() {

    CodeGenerator masm;

#if 0
    masm.mov(RAX, RSI);
    masm.push(RAX);
    masm.pop(RAX);
    masm.cqo();
    masm.idiv(RDI);
    masm.ret();

    masm.push(RDI);
    masm.push(RSI);
#endif
    masm.push(Imm32(11));
    masm.push(Imm32(22));
    masm.pop(RAX);
    masm.pop(RBX);
    masm.add(RAX, RBX);
    masm.ret();

    // rwx protection per page boundary, hence can't use malloc allocated memory
    // which may span pages in unpredictable ways.
    // TODO(dannas): Introduce W^X via mprotect
    // TODO(dannas): Call munmap.
    void *mem = mmap(nullptr, masm.size(), PROT_WRITE | PROT_EXEC,
            MAP_ANON | MAP_PRIVATE, -1, 0);

    memcpy(mem, masm.data(), masm.size());

    int (*func)(int, int) = (int (*)(int, int))mem;

    int sum = func(40, 2);
    printf("sum=%d\n", sum);
    
    return 0;
}
