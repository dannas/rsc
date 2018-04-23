How create a baseline compiler?
We want to use a stack for keeping track of  latent values.
What does  "latent values" mean?

enum Kind {
    STK_REG,
    STK_MEM
};

struct Stk {
    Kind kind;
    union {
        Reg reg;
        uint32_t offset;
    }
};

void sync(vector<Stk> stk) {
    // Store all regs in memory and record the offsets
}

Reg void load(Stk stk) {
    // If reg then return it directly
    // If mem then do mov r, [stk.offset]
}

Reg pop() {

}
