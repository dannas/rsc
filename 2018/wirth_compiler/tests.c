void buf_test() {
    int *buf = NULL;
    assert(buf_cap(buf) == 0);
    assert(buf_len(buf) == 0);
    size_t n = 1024;
    for (size_t i = 0; i < n; i++) {
        buf_push(buf, i);
    }
    assert(buf_len(buf) == n);
    assert(buf_cap(buf) >= n);
}

void intern_test() {
    char a[] = "hello";
    assert(strcmp(a, str_intern(a)) == 0);
    assert(str_intern(a) == str_intern(a));
    assert(str_intern(str_intern(a)) == str_intern(a));
    char b[] = "hello";
    assert(a != b);
    assert(str_intern(a) == str_intern(b));
    char c[] = "hello!";
    assert(str_intern(a) != str_intern(c));
    char d[] = "hell";
    assert(str_intern(a) != str_intern(d));
}

void keyword_test() {
    init_keywords();
    assert(is_keyword_name(first_keyword));
    assert(is_keyword_name(last_keyword));
    for (const char **it = keywords; it != buf_end(keywords); it++) {
        assert(is_keyword_name(*it));
    }
    assert(!is_keyword_name(str_intern("foo")));
}

#define assert_token(kind) assert(match_token(kind))
#define assert_keyword(name) assert(match_keyword(str_intern(name)))

void lex_test() {
    keyword_test();

    // Numbers
    init_stream(NULL, "1 10 20 3000000000");
    assert_token(TOKEN_NUMBER);
    assert_token(TOKEN_NUMBER);
    assert_token(TOKEN_NUMBER);
    assert_token(TOKEN_NUMBER);
    assert_token(TOKEN_EOF);

    // Nested Comments
    init_stream(NULL, "(*(* (*  *)1 *) 2*) * *)");
    assert_token(TOKEN_MULTIPLY);
    assert_token(TOKEN_MULTIPLY);
    assert_token(TOKEN_RPAREN);
    assert_token(TOKEN_EOF);

    // Operators
    init_stream(NULL, "+ - * DIV MOD & OR");
    assert_token(TOKEN_ADD);
    assert_token(TOKEN_MINUS);
    assert_token(TOKEN_MULTIPLY);
    assert_token(TOKEN_DIV);
    assert_token(TOKEN_MOD);
    assert_token(TOKEN_AND);
    assert_token(TOKEN_OR);
    assert_token(TOKEN_EOF);
}

#undef assert_token
#undef assert_keyword

void arena_test() {
    Arena arena = {0};
    size_t n = 1024;
    for (size_t i = 0; i <  n; i = i*2 + 1) {
        char *ptr = arena_alloc(&arena, i);
        printf("%p\n", ptr);
    }
    arena_free(&arena);
}

void test_invertible_encoding(Instruction instr) {
    uint32_t encoded_data = encode_instruction(instr);
    Instruction decoded_instr = decode_instruction(encoded_data);
    assert(memcmp(&instr, &decoded_instr, sizeof(instr)) == 0);
}

void test_decode_encode() {
    for (OpCode op = MOV; op <= CMP; op++) {
        for (int a = 0; a < 16; a++) {
            for (int b = 0; b < 16; b++)  {
                for (int c = 0; c < 16; c++) {
                    test_invertible_encoding((Instruction){.op = op, .a=a, .b=b, .c=c});
                }
            }
        }
    }
    for (OpCode op = MOVI; op <=CMPI; op++) {
        for (int a = 0; a < 16; a++) {
            for (int b = 0; b < 16; b++)  {
                for (int imm = 0; imm < 128; imm++) {
                    test_invertible_encoding((Instruction){.op = op, .a=a, .b=b, .imm=imm});

                }
            }
        }
    }
    for (OpCode op = LDW; op <= PSH; op++) {
        for (int a = 0; a < 16; a++) {
            for (int b = 0; b < 16; b++)  {
                for (int disp = 0; disp < 128; disp++) {
                    test_invertible_encoding((Instruction){.op = op, .a=a, .b=b, .disp=disp});

                }
            }
        }
    }
    for (OpCode op = BEQ; op <= RET; op++) {
        for (int disp = 0; disp < 128; disp++) {
            test_invertible_encoding((Instruction){.op = op, .disp=disp});
        }
    }
}

void run_tests() {

    buf_test();
    intern_test();
    arena_test();
    lex_test();
    test_decode_encode();

    printf("all test passed\n");
}
