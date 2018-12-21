#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "common.c"
#include "risc.c"
#include "lex.c"
#include "parse.c"
#include "tests.c"

void simulator_test() {
    uint8_t ram[4*1024];
    Asm *as = &(Asm){.buf=ram, .buf_size=sizeof(ram)};

    asm_imm_op(as, MOVI, R1, R0, 10);         // 	N=10
    asm_imm_op(as, MOVI, R2, R0, 0);          // 	i=0
    asm_imm_op(as, MOVI, R3, R0, 0);          // 	sum=0
    asm_reg_op(as, CMP, R2, R2, R1);          // 	if i >= N, goto out
    asm_br_op(as, BGE, 5);
                                              // loop:
    asm_reg_op(as, ADD, R3, R3, R2);          // 	sum += i
    asm_imm_op(as, ADDI, R2, R2, 1);          // 	i++
    asm_reg_op(as, CMP, R2, R2, R1);          // 	if i < N, goto loop
    asm_br_op(as, BLT, -3);
                                              // out:
    asm_br_op(as, RET, 0);                    // 	return sum


    Bus bus = { .ram=ram, .ram_start=0, .ram_end=sizeof(ram)};
    Hart hart = { .pc = 0, .N=false, .Z=false, .bus=&bus };
    cmd_loop(&hart);
}

char *read_file(const char *path) {
    FILE *file = fopen(path, "r");
    if (!file) {
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long len = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buf = xmalloc(len + 1);
    if (len && fread(buf, len, 1, file) != 1) {
        free(buf);
        fclose(file);
        return NULL;
    }
    buf[len] = '\0';
    fclose(file);
    return buf;
}

int main() {
    run_tests();
    //simulator_test();

    uint8_t ram[4*1024];
    assembler = &(Asm){.buf=ram, .buf_size=sizeof(ram), .stack_start = 2048, .stack_size=1024};
    char *buf = read_file("oberon.txt");

    init_stream("oberon.txt", buf);

    parse_module();

    Bus bus = { .ram=ram, .ram_start=0, .ram_end=sizeof(ram)};
    Hart hart = { .pc = 0, .bus=&bus };
    cmd_loop(&hart);

    return 0;
}

