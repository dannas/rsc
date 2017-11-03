#include <cstdint>
#include <sys/mman.h>
#include <cstring>
#include <cstdio>

// Encoding x86 instructions
// http://www.c-jump.com/CIS77/CPU/x86/lecture.html

enum Reg : uint8_t{
    EAX = 0,
    ECX = 1,
    EDX = 2,
    EBX = 3,
    ESP = 4,
    EBP = 5,
    ESI = 6,
    EDI = 7
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

uint8_t code[64];

uint8_t* add(uint8_t* buf, Reg dst, Reg src) {
    uint8_t op = 0x01;
    uint8_t mod = REG_REG;
    uint8_t modrm = mod << 6 | src << 3 | dst;
    *buf++ = op;
    *buf++ = modrm;
    return buf;
}

uint8_t* sub(uint8_t* buf, Reg dst, Reg src) {
    uint8_t op = 0x29;
    uint8_t mod = REG_REG;
    uint8_t modrm = mod << 6 | src << 3 | dst;
    *buf++ = op;
    *buf++ = modrm;
    return buf;
}

uint8_t* imul(uint8_t* buf, Reg src) {
    uint8_t op = 0xf7;
    uint8_t mod = REG_REG;
    uint8_t rm = 5;
    uint8_t modrm = mod << 6 | rm << 3 | src;
    *buf++ = op;
    *buf++ = modrm;
    return buf;
}

uint8_t* idiv(uint8_t* buf, Reg src) {
    uint8_t op = 0xf7;
    uint8_t mod = REG_REG;
    uint8_t rm = 7;
    uint8_t modrm = mod << 6 | rm << 3 | src;
    *buf++ = op;
    *buf++ = modrm;
    return buf;
}

uint8_t* ret(uint8_t* buf) {
    uint8_t op = 0xc3;
    *buf++ = op;
    return buf;
}

uint8_t* mov(uint8_t* buf, Reg dst, Reg src) {
    uint8_t op = 0x89;
    uint8_t mod = REG_REG;
    uint8_t modrm = mod << 6 | src << 3 | dst;
    *buf++ = op;
    *buf++ = modrm;
    return buf;
}

int main(int argc, char *argv[]) {
    uint8_t* p = code;

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
    // [reg]
    // [reg + reg] + disp
    p = mov(p, EAX, ESI);
    p = idiv(p, EDI);
    p = ret(p);

    // rwx protection per page boundary, hence can't use malloc allocated memory
    // which may span pages in unpredictable ways.
    // TODO(dannas): Introduce W^X via mprotect
    // TODO(dannas): Call munmap.
    void *mem = mmap(nullptr, sizeof(code), PROT_WRITE | PROT_EXEC,
            MAP_ANON | MAP_PRIVATE, -1, 0);

    memcpy(mem, code, sizeof(code));

    int (*func)(int, int) = (int (*)(int, int))mem;

    int sum = func(2, 16);
    printf("sum=%d\n", sum);
    
    return 0;
}
