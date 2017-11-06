#include <cstdint>
#include <sys/mman.h>
#include <cstring>
#include <cstdio>
#include <vector>

using namespace std;

// Encoding x86 instructions
// http://www.c-jump.com/CIS77/CPU/x86/lecture.html

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

// e1   11 100 001

// mod       effective addr
// 00        example [reg] 
// 01                [reg] + disp8
// 10                [reg] + disp32
// 11        reg-reg
enum Mod : uint8_t {
    REG_INDIRECT = 0x00,
    REG_DISP1 = 0x01,
    REG_DISP4 = 0x02,
    REG_REG = 0x3
};

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
    void ret();
    void sub(Reg dst, Reg src);

private:
    vector<uint8_t> buf_;
};

uint8_t* CodeGenerator::data() {
    return buf_.data();
}

size_t CodeGenerator::size() {
    return buf_.size();
}

void CodeGenerator::add(Reg dst, Reg src) {
    uint8_t pre = 0x48;
    uint8_t op = 0x01;
    uint8_t mod = REG_REG;
    uint8_t modrm = mod << 6 | src << 3 | dst;

    buf_.push_back(pre);
    buf_.push_back(op);
    buf_.push_back(modrm);
}

void CodeGenerator::cqo() {
    uint8_t pre = 0x48;
    uint8_t op = 0x99;

    buf_.push_back(pre);
    buf_.push_back(op);
}

void CodeGenerator::sub(Reg dst, Reg src) {
    uint8_t pre = 0x48;
    uint8_t op = 0x29;
    uint8_t mod = REG_REG;
    uint8_t modrm = mod << 6 | src << 3 | dst;

    buf_.push_back(pre);
    buf_.push_back(pre);
    buf_.push_back(op);
    buf_.push_back(modrm);
}

void CodeGenerator::imul(Reg src) {
    uint8_t pre = 0x48;
    uint8_t op = 0xf7;
    uint8_t mod = REG_REG;
    uint8_t rm = 5;
    uint8_t modrm = mod << 6 | rm << 3 | src;

    buf_.push_back(pre);
    buf_.push_back(op);
    buf_.push_back(modrm);
}

void CodeGenerator::idiv(Reg src) {
    uint8_t pre = 0x48;
    uint8_t op = 0xf7;
    uint8_t mod = REG_REG;
    uint8_t rm = 7;
    uint8_t modrm = mod << 6 | rm << 3 | src;

    buf_.push_back(pre);
    buf_.push_back(op);
    buf_.push_back(modrm);
}

void CodeGenerator::ret() {
    uint8_t op = 0xc3;

    buf_.push_back(op);
}

void CodeGenerator::mov(Reg dst, Reg src) {
    uint8_t pre = 0x48;
    uint8_t op = 0x89;
    uint8_t mod = REG_REG;
    uint8_t modrm = mod << 6 | src << 3 | dst;

    buf_.push_back(pre);
    buf_.push_back(op);
    buf_.push_back(modrm);
}

void CodeGenerator::push(Reg src) {
    buf_.push_back(0x50 | src);
}

void CodeGenerator::pop(Reg dst) {
    buf_.push_back(0x58 | dst);
}

int main() {

    // TODO(dannas): Should we keep addresses in registers or on the stack?
    // Is it easier to use the stack?
    // How do I allocate registers?
    
    // TODO(dannas): What asm instructions do I need for executing P-code?
    // arithmetic operators
    // loading locals
    // calling and returning from functions
    // printing return values
    
    // TODO(dannas): How distinguish between different "addressing forms"?
    // * reg-reg
    // * reg-imm
    // * reg-[reg]
    // * reg-[reg + reg] + disp
    CodeGenerator masm;

    masm.mov(RAX, RSI);
    masm.push(RAX);
    masm.pop(RAX);
    masm.cqo();
    masm.idiv(RDI);
    masm.ret();

    // rwx protection per page boundary, hence can't use malloc allocated memory
    // which may span pages in unpredictable ways.
    // TODO(dannas): Introduce W^X via mprotect
    // TODO(dannas): Call munmap.
    void *mem = mmap(nullptr, masm.size(), PROT_WRITE | PROT_EXEC,
            MAP_ANON | MAP_PRIVATE, -1, 0);

    memcpy(mem, masm.data(), masm.size());

    int (*func)(int, int) = (int (*)(int, int))mem;

    int sum = func(2, 16);
    printf("sum=%d\n", sum);
    
    return 0;
}
