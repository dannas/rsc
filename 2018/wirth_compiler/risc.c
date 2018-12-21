typedef enum InstrKind {
    REG_REG = 0x00,
    REG_IMM = 0x10,
    MEM     = 0x20,
    BRANCH  = 0x30,
} InstrKind;

typedef enum OpCode {
    ILLEGAL = 0,
    MOV = REG_REG | 1,
    MVN = REG_REG | 2,
    ADD = REG_REG | 3,
    SUB = REG_REG | 4,
    MUL = REG_REG | 5,
    DIV = REG_REG | 6,
    MOD = REG_REG | 7,
    CMP = REG_REG | 8,

    MOVI = REG_IMM | 0,
    MVNI = REG_IMM | 1,
    ADDI = REG_IMM | 2,
    SUBI = REG_IMM | 3,
    MULI = REG_IMM | 4,
    DIVI = REG_IMM | 5,
    MODI = REG_IMM | 6,
    CMPI = REG_IMM | 7,

    LDW = MEM | 0,
    LDB = MEM | 1,
    POP = MEM | 2,
    STW = MEM | 3,
    STB = MEM | 4,
    PSH = MEM | 5,

    BEQ = BRANCH | 0,
    BLT = BRANCH | 1,
    BLE = BRANCH | 2,
    BNE = BRANCH | 3,
    BGE = BRANCH | 4,
    BGT = BRANCH | 5,
    BR  = BRANCH | 6,
    BSR = BRANCH | 7,
    RET = BRANCH | 8,
} OpCode;

const char* op_to_name[] = {
    [MOV] = "MOV",
    [MVN] = "MVN",
    [ADD] = "ADD",
    [SUB] = "SUB",
    [MUL] = "MUL",
    [DIV] = "DIV",
    [MOD] = "MOD",
    [CMP] = "CMP",

    [MOVI] = "MOVI",
    [MVNI] = "MVNI",
    [ADDI] = "ADDI",
    [SUBI] = "SUBI",
    [MULI] = "MULI",
    [DIVI] = "DIVI",
    [MODI] = "MODI",
    [CMPI] = "CMPI",

    [LDW] = "LDW",
    [LDB] = "LDB",
    [POP] = "POP",
    [STW] = "STW",
    [STB] = "STB",
    [PSH] = "PSH",

    [BEQ] = "BEQ",
    [BLT] = "BLT",
    [BLE] = "BLE",
    [BNE] = "BNE",
    [BGE] = "BGE",
    [BGT] = "BGT",
    [BR ] = "BR",
    [BSR] = "BSR",
    [RET] = "RET",
};

typedef enum Reg {
    R0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15,
    NUM_REGS,

    ZERO_REG = R0,
    FP = R12,
    SP = R13,
    LR = R14,
    PC = R15,
} Reg;

typedef struct Instruction {
    OpCode op;
    Reg a, b, c;
    int32_t imm;
    int32_t disp;
} Instruction;

uint32_t bits(uint32_t data, uint32_t start, uint32_t len) {
    return (data >> start) & ((1<< len) - 1);
}

// Hackers Delight 2-6.
uint32_t sign_extend(uint32_t data, uint32_t len) {
    uint32_t mask = 1<< (len-1);
    return (data ^ mask) - mask;
}

Instruction decode_instruction(uint32_t data) {
    OpCode op = bits(data, 26, 6);
    uint8_t a = bits(data, 22, 4);
    uint8_t b = bits(data, 18, 4);
    uint8_t c = bits(data, 0, 4);
    uint32_t imm_disp = bits(data, 0, 18);
    imm_disp = sign_extend(imm_disp, 18);
    uint32_t disp = bits(data, 0, 26);
    disp = sign_extend(disp, 26);

    switch (op) {
    case MOV...CMP:
        return (Instruction){ .op = op, .a = a, .b = b, .c = c };
        break;
    case MOVI...CMPI:
        return (Instruction){ .op = op, .a = a, .b = b, .imm = imm_disp };
        break;
    case LDW...PSH:
        return (Instruction){ .op = op, .a = a, .b = b, .disp = imm_disp };
        break;
    case BEQ...RET:
        return (Instruction){ .op = op, .disp = disp };
        break;
    default:
        assert(0);
    }
}

uint32_t encode_instruction(Instruction instr) {
    assert(instr.a >= 0 && instr.a < 32);
    assert(instr.b >= 0 && instr.b < 32);
    assert(instr.c >= 0 && instr.c < 32);

    switch (instr.op) {
    case MOV...CMP:
        return instr.op << 26 | instr.a << 22 | instr.b << 18 | instr.c;
        break;
    case MOVI...CMPI:
        return instr.op << 26 | instr.a << 22 | instr.b << 18 | (instr.imm & ((1 << 18) -1));
        break;
    case LDW...PSH:
        return instr.op << 26 | instr.a << 22 | instr.b << 18 | (instr.disp & ((1 << 18) -1));
        break;
    case BEQ...RET:
        return instr.op << 26 | (instr.disp & ((1 << 26) -1));
        break;
    default:
        assert(0);
    }
}


typedef struct Bus {
    uint8_t* ram;
    uint32_t ram_start;
    uint32_t ram_end;
} Bus;

uint32_t bus_load_word(Bus *bus, uint32_t addr) {
    if (addr >= bus->ram_start && addr + 4 <= bus->ram_end) {
        return *(uint32_t*)(bus->ram + addr - bus->ram_start);
    } else {
        return 0;
    }
}

uint8_t bus_load_byte(Bus *bus, uint32_t addr) {
    if (addr >= bus->ram_start && addr + 1 <= bus->ram_end) {
        return *(uint8_t*)(bus->ram + addr - bus->ram_start);
    } else {
        return 0;
    }
}

void bus_store_word(Bus *bus, uint32_t addr, uint32_t val) {
    if (addr >= bus->ram_start && addr + 4 <= bus->ram_end) {
        // TODO(dannas): Aren't we violating strict aliasing if we write both bytes and words to ram?
        *(uint32_t*)(bus->ram + addr - bus->ram_start) = val;
    }
}

void bus_store_byte(Bus *bus, uint32_t addr, uint8_t val) {
    if (addr >= bus->ram_start && addr + 1 <= bus->ram_end) {
        // TODO(dannas): Aren't we violating strict aliasing if we write both bytes and words to ram?
        *(uint8_t*)(bus->ram + addr - bus->ram_start) = val;
    }
}


typedef struct Hart {
    uint32_t pc;
    uint32_t regs[NUM_REGS];
    bool N;
    bool Z;
    Bus *bus;
} Hart;

uint32_t load_word(Hart *hart, uint32_t addr) {
    return bus_load_word(hart->bus, addr);
}

uint8_t load_byte(Hart *hart, uint32_t addr) {
    return bus_load_byte(hart->bus, addr);
}

void store_word(Hart *hart, uint32_t addr, uint32_t val) {
    return bus_store_word(hart->bus, addr, val);
}

void store_byte(Hart *hart, uint32_t addr, uint32_t val) {
    return bus_store_byte(hart->bus, addr, val);
}

void write_reg(Hart *hart, Reg reg, uint32_t val) {
    hart->regs[reg] = val;
}

uint32_t fetch_instruction(Hart *hart, uint32_t addr) {
    return bus_load_word(hart->bus, addr);
}

void step(Hart *hart) {
    uint32_t pc = hart->pc;
    uint32_t instr_data = fetch_instruction(hart, pc);
    Instruction instr = decode_instruction(instr_data);

    OpCode op = instr.op;
    Reg a = instr.a;
    Reg b = instr.b;
    Reg c = instr.c;
    uint32_t imm = instr.imm;
    // TODO(dannas): Adjust
    int32_t disp = instr.disp;
    uint32_t a_val = hart->regs[a];
    uint32_t b_val = hart->regs[b];
    uint32_t c_val = hart->regs[c];
    bool N = hart->N;
    bool Z = hart->Z;
    uint32_t next_pc = pc + 4;
    uint32_t branch_pc = pc + (disp * 4);

    switch (op) {
    case MOV:
        write_reg(hart, a, c_val << b_val);
        break;
    case MVN:
        write_reg(hart, a, -(c_val << b_val));
        break;
    case ADD:
        write_reg(hart, a, b_val + c_val);
        break;
    case SUB:
        write_reg(hart, a, b_val - c_val);
        break;
    case MUL:
        write_reg(hart, a, b_val * c_val);
        break;
    case DIV:
        write_reg(hart, a, b_val / c_val);
        break;
    case MOD:
        write_reg(hart, a, b_val % c_val);
        break;
    case CMP:
        hart->Z = b_val == c_val;
        hart->N = b_val < c_val;
        break;

    case MOVI:
        write_reg(hart, a, imm << b_val);
        break;
    case MVNI:
        write_reg(hart, a, -(imm << b_val));
        break;
    case ADDI:
        write_reg(hart, a, b_val + imm);
        break;
    case SUBI:
        write_reg(hart, a, b_val - imm);
        break;
    case MULI:
        write_reg(hart, a, b_val * imm);
        break;
    case DIVI:
        write_reg(hart, a, b_val / imm);
        break;
    case MODI:
        write_reg(hart, a, b_val % imm);
        break;
    case CMPI:
        hart->Z = b_val == imm;
        hart->N = b_val < imm;
        break;

    case LDW:
        write_reg(hart, a, load_word(hart, b_val + disp));
        break;
    case LDB:
        write_reg(hart, a, load_byte(hart, b_val + disp));
        break;
    case POP:
        write_reg(hart, b, b_val - disp);
        write_reg(hart, a, load_word(hart, b_val - disp));
        break;
    case STW:
        store_word(hart, b_val + disp, a_val);
        break;
    case STB:
        store_byte(hart, b_val + disp, a_val);
        break;
    case PSH:
        store_word(hart, b_val, a_val);
        write_reg(hart, b, b_val + disp);
        break;

    case BEQ:
        if (Z) {
            next_pc = branch_pc;
        }
        break;
    case BLT:
        if (N) {
            next_pc = branch_pc;
        }
        break;
    case BLE:
        if (N || Z) {
            next_pc = branch_pc;
        }
        break;
    case BNE:
        if (!Z) {
            next_pc = branch_pc;
        }
        break;
    case BGE:
        if (!N) {
            next_pc = branch_pc;
        }
        break;
    case BGT:
        if (!(N || Z)) {
            next_pc = branch_pc;
        }
        break;
    case BR :
        next_pc = branch_pc;
        break;
    case BSR:
        write_reg(hart, LR, next_pc);
        next_pc = branch_pc;
        break;
    case RET:
        next_pc = hart->regs[disp % NUM_REGS];
        // TODO(dannas): Exit if disp == 0
        break;
    default:
        assert(0);
    }
    hart->pc = next_pc;
}

void print_instruction(char *buf, uint32_t pc, Instruction instr) {
    switch (instr.op) {
    case MOV...CMP:
        sprintf(buf, "%s r%d, r%d, r%d", op_to_name[instr.op], instr.a, instr.b, instr.c);
        break;
    case MOVI...CMPI:
        sprintf(buf, "%s r%d, r%d, %d", op_to_name[instr.op], instr.a, instr.b, instr.imm);
        break;
    case LDW...PSH:
        sprintf(buf, "%s r%d, [r%d%+d]", op_to_name[instr.op], instr.a, instr.b, instr.disp);
        break;
    case BEQ...RET:
        sprintf(buf, "%s %d", op_to_name[instr.op], pc + (instr.disp * 4));
        break;
    default:
        assert(0);
    }
}

void print_hart_state(Hart *hart) {
    char instr_str[128];
    uint32_t instr_data = fetch_instruction(hart, hart->pc);
    Instruction instr = decode_instruction(instr_data);
    print_instruction(instr_str, hart->pc, instr);
    printf("%s\n\n", instr_str);
    printf("pc=%d (0x%x)\n", hart->pc, hart->pc);
    for (Reg reg = R0; reg < NUM_REGS; reg++) {
        if (reg && reg % 4 == 0) {
            printf("\n");
        }
        printf("r%-2d=%4d | ", reg, hart->regs[reg]);
    }
    printf("\n");
}

void cmd_loop(Hart *hart) {
    print_hart_state(hart);
    while (true) {
        getchar();
        step(hart);
        print_hart_state(hart);
    }
}

typedef struct Asm {
    uint8_t *buf;
    uint32_t addr;
    uint32_t buf_size;
    uint32_t stack_start;
    uint32_t stack_addr;
    uint32_t stack_size;
} Asm;

void asm_bytes(Asm *as, void *bytes, uint32_t size) {
    if (as->addr + size <= as->buf_size) {
        memcpy(as->buf + as->addr, bytes, size);
        as->addr += size;
    } else {
        fatal("asm_bytes not enough space for bytes");
    }
}

void asm_uint32(Asm *as, uint32_t data) {
    asm_bytes(as, &data, 4);
}

void asm_instr(Asm *as, Instruction instr) {
    asm_uint32(as, encode_instruction(instr));
}

void asm_reg_op(Asm *as, OpCode op, Reg r1, Reg r2, Reg r3) {
    asm_instr(as, (Instruction){.op=op, .a=r1, .b=r2, .c=r3});
}

void asm_imm_op(Asm *as, OpCode op, Reg r1, Reg r2, uint32_t imm) {
    asm_instr(as, (Instruction){.op=op, .a=r1, .b=r2, .imm=imm});
}

void asm_mem_op(Asm *as, OpCode op, Reg r1, Reg r2, uint32_t disp) {
    asm_instr(as, (Instruction){.op=op, .a=r1, .b=r2, .disp=disp});
}

void asm_br_op(Asm *as, OpCode op, uint32_t disp) {
    asm_instr(as, (Instruction){.op=op, .disp=disp});
}

void asm_ldw_var(Asm *as, Reg r1, Reg r2, uint32_t var_index) {
    // TODO(dannas): Replace this
    uint32_t disp = as->stack_start + var_index;
    assert (disp + 4 <= as->stack_start + as->stack_size);
    asm_mem_op(as, LDW, r1, r2, disp);
}

void asm_stw_var(Asm *as, Reg r1, Reg r2, uint32_t var_index) {
    // TODO(dannas): Replace this
    uint32_t disp = as->stack_start + var_index;
    assert (disp + 4 <= as->stack_start + as->stack_size);
    asm_mem_op(as, STW, r1, r2, disp);
}

void asm_cond_set(Asm *as, OpCode cond, Reg r1, Reg r2) {
    // TODO(dannas): Use labels for these once implemented
    asm_reg_op(as, CMP, r1, r1, r2);
    asm_br_op(as, cond, 3);
    asm_imm_op(as, MOVI, r1, R0, 0);
    asm_br_op(as, BR, 2);
    asm_imm_op(as, MOVI, r1, R0, 1);
}

void asm_and(Asm *as, Reg r1, Reg r2) {
    asm_imm_op(as, CMPI, r1, r1, 0);
    asm_br_op(as, BEQ, 5);
    asm_imm_op(as, CMPI, r2, r2, 0);
    asm_br_op(as, BEQ, 3);
    asm_imm_op(as, MOVI, r1, R0, 1);
    asm_br_op(as, BR, 2);
    asm_imm_op(as, MOVI, r1, R0, 0);
}
