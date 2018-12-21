
/// ident = letter {letter | digit}.
/// integer = digit {digit}.
/// selector = {"." ident | "[" expression "]"}.
/// number = integer.
/// factor = ident selector | number | "(" expression ")" | "~" factor.
/// term = factor {("*" | "DIV" | "MOD" | "&") factor}.
/// SimpleExpression = ["+"|"-"] term {("+"|"-" | "OR") term}.
/// expression = SimpleExpression
/// 				[("=" | "#" | "<" | "<=" | ">" | ">=") SimpleExpression].

/// assignment = ident selector ":=" expression.
/// ActualParameters = "(" [expression {"," expression}] ")" .
/// ProcedureCall = ident selector [ActualParameters].
/// IfStatement = "IF" expression "THEN" StatementSequence
/// 				 {"ELSIF" expression "THEN" StatementSequence}
/// 				 ["ELSE" StatementSequence] "END".
/// WhileStatement = "WHILE" expression "DO" StatementSequence "END".
/// statement = [assignment | ProcedureCall | IfStatement | WhileStatement].
/// StatementSequence = statement {";" statement}.
//
/// IdentList = ident {"," ident}.
/// ArrayType = "ARRAY" expression "OF" type.
/// FieldList = [IdentList ":" type].
/// RecordType = "RECORD" FieldList {";" FieldList} "END".
/// type = ident | ArrayType | RecordType.
/// FPSection = ["VAR"] IdentList ":" type.
/// FormalParameters = "(" [FPSection {";" FPSection}] ")".
/// ProcedureHeading = "PROCEDURE" ident [FormalParameters].
/// ProcedureBody = declarations ["BEGIN" StatementSequence] "END" ident.
/// ProcedureDeclaration = ProcedureHeading ";" ProcedureBody.
/// declarations = ["CONST" {ident "=" expression ";"}]
/// 				  ["TYPE" {ident "=" type ";"}]
/// 				  ["VAR" {IdentList ":" type ";"}]
/// 				  { ProcedureDeclaration ";"}.
/// module = "MODULE" ident ";" declarations
/// 			["BEGIN" StatementSequence] "END" ident "." .

typedef enum TypeKind {
    TYPE_NONE,
    TYPE_INT,
    TYPE_BOOL,
    TYPE_PROC,
    TYPE_ARRAY,
    TYPE_RECORD,
    NUM_TYPE_KINDS,
} TypeKind;

typedef struct Type {
    TypeKind kind;
} Type;

typedef enum SymKind {
    SYM_NONE,
    SYM_TYPE,
    SYM_CONST,
    SYM_VAR,
    SYM_PROC,
    SYM_MODULE,
} SymKind;

typedef struct Sym {
    SymKind kind;
    const char *name;
    Type type;
} Sym;

enum {
    MAX_LOCAL_SYMS = 8,
};
Sym local_syms[MAX_LOCAL_SYMS];
Sym *local_syms_end = local_syms;

Sym* sym_get_local(const char *name) {
    for (Sym *it = local_syms_end; it != local_syms; it--) {
        Sym *sym = it-1;
        if (sym->name == name) {
            return sym;
        }
    }
    return NULL;
}

bool sym_push_var(const char *name, Type type) {
    if (sym_get_local(name)) {
        return false;
    }
    *local_syms_end++ = (Sym){
        .kind = SYM_VAR,
        .name = name,
        .type = type,
    };
    return true;
}

Sym* sym_enter() {
    return local_syms_end;
}

void sym_leave(Sym *sym) {
    local_syms_end = sym;
}

Asm *assembler;
// The evaluation of expressions is done via an implicit stack of registers.
// |current_reg| acts as a stack pointer.
// R0 is reserved as a zero constant.
Reg current_reg = R1;

void parse_expr();

void parse_selector() {
    while (is_token(TOKEN_DOT) || is_token(TOKEN_LBRACKET)) {
        if (is_token(TOKEN_DOT)) {
            // TODO(dannas): Generate asm for selector
            next_token();
            expect_token(TOKEN_NAME);
        }  else if (is_token(TOKEN_LBRACKET)) {
            next_token();
            parse_expr();
            expect_token(TOKEN_RBRACKET);
        }
    }
}
void parse_factor() {
    if (is_token(TOKEN_NUMBER)) {
        asm_imm_op(assembler, MOVI, current_reg, R0, token.number);
        current_reg++;
        next_token();
    } else if (is_token(TOKEN_NAME)) {
        if (!sym_get_local(token.name)) {
            fatal_error(token.pos, "'%s' is not defined", token.name);
        }
        // TODO(dannas): Lookup index of variable instead of hardcoded 0
        asm_ldw_var(assembler, current_reg, R0, 0);
        next_token();
        parse_selector();
    } else if (is_token(TOKEN_LPAREN)) {
        next_token();
        parse_expr();
        expect_token(TOKEN_RPAREN);
    } else if (is_token(TOKEN_NOT)) {
        next_token();
        parse_factor();
    } else {
        fatal_error_here("Expected number or ( or ~ but got %s", token_kind_name(token.kind));
    }
}
void parse_term() {
    parse_factor();
    while (is_token(TOKEN_MULTIPLY) || is_token(TOKEN_DIV) || is_token(TOKEN_MOD) || is_token(TOKEN_AND)) {
        TokenKind op = token.kind;
        next_token();
        parse_factor();
        if (op == TOKEN_MULTIPLY) {
            asm_reg_op(assembler, MUL, current_reg-2, current_reg-2, current_reg-1);
        } else if (op == TOKEN_DIV) {
            asm_reg_op(assembler, DIV, current_reg-2, current_reg-2, current_reg-1);
        } else if (op == TOKEN_MOD) {
            asm_reg_op(assembler, MOD, current_reg-2, current_reg-2, current_reg-1);
        } else if (op == TOKEN_AND) {
            asm_and(assembler, current_reg-2, current_reg-1);
        }
        current_reg--;
    }
}

void parse_simple_expr() {
    // TODO(dannas): Do we need prefix +- or can they be placed under unary?
    parse_term();
    while (is_token(TOKEN_ADD) || is_token(TOKEN_MINUS) || is_token(TOKEN_OR)) {
        TokenKind op = token.kind;
        next_token();
        parse_term();
        if (op == TOKEN_ADD) {
            asm_reg_op(assembler, ADD, current_reg-2, current_reg-2, current_reg-1);
        } else if (op == TOKEN_MINUS) {
            asm_reg_op(assembler, SUB, current_reg-2, current_reg-2, current_reg-1);
        } else if (op == TOKEN_OR) {
            ; // TODO(dannas): Generate asm for OR
        }
        current_reg--;
    }
}

void parse_expr() {
    parse_simple_expr();

    if (is_token(TOKEN_EQ) || is_token(TOKEN_NE) || is_token(TOKEN_LT) || is_token(TOKEN_GT) || is_token(TOKEN_GE)) {
        TokenKind op = token.kind;
        next_token();
        parse_simple_expr();

        if (op == TOKEN_EQ) {
            asm_cond_set(assembler, BEQ, current_reg-2, current_reg-1);
        } else if(op == TOKEN_NE) {
            asm_cond_set(assembler, BNE, current_reg-2, current_reg-1);
        } else if (op == TOKEN_LT) {
            asm_cond_set(assembler, BLT, current_reg-2, current_reg-1);
        } else if (op == TOKEN_LE) 	{
            asm_cond_set(assembler, BLE, current_reg-2, current_reg-1);
        } else if (op == TOKEN_GT) {
            asm_cond_set(assembler, BGT, current_reg-2, current_reg-1);
        } else if (op == TOKEN_GE) {
            asm_cond_set(assembler, BGE, current_reg-2, current_reg-1);
        }
        current_reg--;
    }
}

void parse_assignment() {
    if (is_token(TOKEN_NAME)) {
        next_token();
        parse_selector();
        expect_token(TOKEN_COLON_ASSIGN);
        parse_expr();
        asm_stw_var(assembler, current_reg-1, R0, 0);
        current_reg--;
    }
}

void parse_actual_params() {
    expect_token(TOKEN_LPAREN);
    while (!is_token(TOKEN_RPAREN)) {
        parse_expr();
        while (match_token(TOKEN_COMMA)) {
            parse_expr();
        }
    }
    expect_token(TOKEN_RPAREN);
}

void parse_proc_call() {
    expect_token(TOKEN_NAME);
    parse_selector();
    if (is_token(TOKEN_LPAREN)) {
        parse_actual_params();
    }
}

void parse_stmt_sequence();

void parse_if_stmt() {
    expect_keyword(IF_keyword);
    parse_expr();
    expect_keyword(THEN_keyword);
    parse_stmt_sequence();
    if (match_keyword(ELSEIF_keyword)) {
        parse_expr();
        expect_keyword(THEN_keyword);
        parse_stmt_sequence();
    }
    if (match_keyword(ELSE_keyword)) {
        parse_stmt_sequence();
    }
    expect_keyword(END_keyword);
}


void parse_while_stmt() {
    expect_keyword(WHILE_keyword);
    parse_expr();
    expect_keyword(DO_keyword);
    parse_stmt_sequence();
    expect_keyword(END_keyword);
}

void parse_stmt() {
    if (is_token(TOKEN_NAME)) {
        // TODO(dannas): Differentiate between assign and proc via symbol type
        if (true) {
            parse_assignment();
        } else {
            parse_proc_call();
        }
    } else if(is_keyword(IF_keyword)) {
        parse_if_stmt();
    } else if(is_keyword(WHILE_keyword)) {
        parse_while_stmt();
    }
}

void parse_stmt_sequence() {
    parse_stmt();
    while(match_token(TOKEN_SEMICOLON)) {
        parse_stmt();
    }
}

void parse_ident_list() {
    if (!is_token(TOKEN_NAME)) {
        fatal_error_here("Expected name but got '%s'", token_info());
    }
    sym_push_var(token.name, (Type){.kind = TYPE_NONE});
    next_token();
    while (match_token(TOKEN_COMMA)) {
        if (!is_token(TOKEN_NAME)) {
            fatal_error_here("Expected name but got '%s'", token_info());
        }
        sym_push_var(token.name, (Type){.kind = TYPE_NONE});
        next_token();
    }
}

Type parse_type();

void parse_array_type() {
    expect_keyword(ARRAY_keyword);
    parse_expr();
    expect_keyword(OF_keyword);
    parse_type();
}

void parse_field_list() {
    if (is_token(TOKEN_NAME)) {
        parse_ident_list();
        expect_token(TOKEN_COLON);
        parse_type();
    }
}

void parse_record_type() {
    expect_keyword(RECORD_keyword);
    parse_field_list();
    while (match_token(TOKEN_SEMICOLON)) {
        parse_field_list();
    }
    expect_keyword(END_keyword);
}

Type parse_type() {
    Type type;
    if (is_token(TOKEN_NAME)) {
        // TODO(dannas): Set right type here
        type.kind = TYPE_NONE;
        next_token();
    } else if (is_keyword(ARRAY_keyword)) {
        type.kind = TYPE_ARRAY;
        parse_array_type();
    } else if (is_keyword(RECORD_keyword)) {
        type.kind = TYPE_RECORD;
        parse_record_type();
    } else {
        fatal_error_here("Error Expected name, ARRAY or RECORD but got %s", token_kind_name(token.kind));
    }
    return type;
}

void parse_fp_section() {
    // TODO(dannas): VAR is optional.Adjust here and in parse_formal_params
    expect_keyword(VAR_keyword);
    parse_ident_list();
    expect_token(TOKEN_COLON);
    parse_type();
}

void parse_formal_params() {
    expect_token(TOKEN_LPAREN);
    if (is_keyword(VAR_keyword)) {
        parse_fp_section();
        while (is_token(TOKEN_SEMICOLON)) {
            next_token();
            parse_fp_section();
        }
    }
    expect_token(TOKEN_RPAREN);
}

void parse_proc_heading() {
    match_keyword(PROCEDURE_keyword);
    expect_token(TOKEN_NAME);
    if (is_token(TOKEN_LPAREN)) {
        parse_formal_params();
    }
}

void parse_declarations();
void parse_proc_body() {
    Sym *first = sym_enter();

    parse_declarations();
    if (match_keyword(BEGIN_keyword)) {
        parse_stmt_sequence();
    }
    expect_keyword(END_keyword);
    expect_token(TOKEN_NAME);

    sym_leave(first);
}

void parse_proc_declaration() {
    parse_proc_heading();
    expect_token(TOKEN_SEMICOLON);
    parse_proc_body();
}


void parse_declarations() {
    if (match_keyword(CONST_keyword)) {
        while (is_token(TOKEN_NAME)) {
            next_token();
            expect_token(TOKEN_EQ);
            parse_expr();
            expect_token(TOKEN_SEMICOLON);
        }
    }
    if (match_keyword(TYPE_keyword)) {
        while (is_token(TOKEN_NAME)) {
            next_token();
            expect_token(TOKEN_EQ);
            parse_type();
            expect_token(TOKEN_SEMICOLON);
        }
    }
    if (match_keyword(VAR_keyword)) {
        while (is_token(TOKEN_NAME)) {
            Sym *first = sym_enter();
            parse_ident_list();
            expect_token(TOKEN_COLON);
            Type type = parse_type();
            for (Sym *sym = first; sym < local_syms_end; sym++) {
                sym->type = type;
            }
            expect_token(TOKEN_SEMICOLON);
        }
    }
    while (is_keyword(PROCEDURE_keyword)) {
        parse_proc_declaration();
        expect_token(TOKEN_SEMICOLON);
    }
}

void parse_module() {
    expect_keyword(MODULE_keyword);
    if (is_token(TOKEN_NAME)) {
        // TODO(dannas): Store name
        next_token();
        expect_token(TOKEN_SEMICOLON);
        parse_declarations();

        if (is_keyword(BEGIN_keyword)) {
            next_token();
            parse_stmt_sequence();
        }
        expect_keyword(END_keyword);
        // TODO(dannas): Store identifier
        expect_token(TOKEN_NAME);
        expect_token(TOKEN_DOT);
    } else {
        fatal_error_here("Error expected name but got %s", token_kind_name(token.kind));
    }
}
