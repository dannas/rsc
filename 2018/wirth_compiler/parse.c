
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

typedef struct Type Type;

typedef struct TypeField {
    Type *type;
    const char *name;
    size_t offset;
} TypeField;

struct Type {
    TypeKind kind;
    size_t size;
    struct Type *base;

    union {
        size_t num_elems;
        struct {
            TypeField *fields;
            size_t num_fields;
        } record;
    };
};

typedef enum SymKind {
    SYM_NONE,
    SYM_TYPE,
    SYM_CONST,
    SYM_VAR,
    SYM_PROC,
    SYM_MODULE,
} SymKind;

char* sym_kind_names[] = {
    [SYM_NONE] = "NONE",
    [SYM_TYPE] = "TYPE",
    [SYM_CONST] = "CONSTANT",
    [SYM_VAR] = "VARABLE",
    [SYM_PROC] = "PROCEDURE",
    [SYM_MODULE] = "MODULE",
};

char* sym_kind_name(SymKind kind) {
    if (kind < sizeof(sym_kind_names)/sizeof(sym_kind_names[0])) {
        return sym_kind_names[kind];
    }
    return "unknown";
}

typedef struct Sym {
    SymKind kind;
    const char *name;
        Type *type;
    union {
        int val;
        size_t var_index;
    };
} Sym;

enum {
    MAX_SYMBOLS = 4096,
};
Sym symbols[MAX_SYMBOLS];
Sym *symbols_end = symbols;
size_t var_index;

Sym* sym_get(const char *name) {
    for (Sym *it = symbols_end; it != symbols; it--) {
        Sym *sym = it-1;
        if (sym->name == name) {
            return sym;
        }
    }
    return NULL;
}

bool sym_push(const char *name, SymKind kind, Type *type) {
    if (sym_get(name)) {
        return false;
    }
    if (symbols_end == symbols + MAX_SYMBOLS) {
        fatal("Too many local symbols");
    }
    *symbols_end++ = (Sym){
        .kind = kind,
        .name = name,
        .type = type,
    };
    return true;
}

Sym* sym_enter() {
    return symbols_end;
}

void sym_leave(Sym *sym) {
    symbols_end = sym;
}

Type *type_none = &(Type){.kind=TYPE_NONE};
Type *type_integer = &(Type){.kind=TYPE_INT, .size=1};
Type *type_boolean = &(Type){.kind=TYPE_BOOL, .size=1};

void init_builtin_types() {
    sym_push(str_intern("BOOLEAN"), SYM_TYPE, type_boolean);
    sym_push(str_intern("INTEGER"), SYM_TYPE, type_integer);
}

Type* type_alloc(TypeKind kind) {
    Type *type = xcalloc(1, sizeof(Type));
    type->kind = kind;
    return type;
}

Type **cached_array_types;

Type* type_array(Type *base, size_t num_elems) {
    for (Type **it = cached_array_types; it != buf_end(cached_array_types); it++) {
        if ((*it)->kind == TYPE_ARRAY && (*it)->base == base && (*it)->num_elems == num_elems) {
            return *it;
        }
    }
    Type *type = type_alloc(TYPE_ARRAY);
    type->base = base;
    type->num_elems = num_elems;
    type->size = num_elems * base->size;
    buf_push(cached_array_types, type);
    return type;
}

Asm *assembler;
// The evaluation of expressions is done via an implicit stack of registers.
// |current_reg| acts as a stack pointer.
// R0 is reserved as a zero constant.
Reg current_reg = R1;

void parse_expr();

void parse_selector(Type *type) {
    while (is_token(TOKEN_DOT) || is_token(TOKEN_LBRACKET)) {
        if (is_token(TOKEN_DOT)) {
            if (type->kind != TYPE_RECORD) {
                fatal_error_here("Dot selector, but not a record");
            }
            // TODO(dannas): Generate asm for selector
            next_token();
            if (!is_token(TOKEN_NAME)) {
                fatal_error_here("Expected a name");
            }
            TypeField *field = NULL;
            for (field = type->record.fields; field < type->record.fields + type->record.num_fields; field++) {
                if (field->name == token.name) {
                    break;
                }
            }
            if (!field) {
                fatal_error_here("'%s' is not a known type field", token.name);
            }
            asm_imm_op(assembler, MOVI, current_reg, R0, field->offset);
            current_reg++;

            next_token();
        }  else if (is_token(TOKEN_LBRACKET)) {
            if (type->kind != TYPE_ARRAY) {
                fatal_error_here("Bracket selectors, but not an array");
            }
            next_token();
            parse_expr();
            asm_imm_op(assembler, MULI, current_reg-1, current_reg-1, type->size / type->num_elems);
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
        Sym *sym = sym_get(token.name);
        if (!sym) {
            fatal_error_here("'%s' is not defined", token.name);
        }
        next_token();
        parse_selector(sym->type);
        if (sym->kind == SYM_CONST) {
            asm_imm_op(assembler, MOVI, current_reg, R0, sym->val);
            current_reg++;
        } else if (sym->kind == SYM_VAR) {
            if (sym->type->kind == TYPE_ARRAY || sym->type->kind == TYPE_RECORD) {
                asm_ldw_var(assembler, current_reg-1, current_reg-1, sym->var_index);
            } else {
                asm_ldw_var(assembler, current_reg, R0, sym->var_index);
                current_reg++;
            }
        } else {
            fatal_error_here("Expected variable or constant but got '%s'", sym_kind_name(sym->kind));
        }
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
        Sym *sym = sym_get(token.name);
        if (!sym) {
            fatal_error_here("'%s' is not defined", token.name);
        }
        if (sym->kind != SYM_VAR) {
            fatal_error_here("'%s' must be VAR but is a %s", sym->name, sym_kind_name(sym->kind));
        }
        next_token();
        TypeKind kind = sym->type->kind;
        parse_selector(sym->type);
        expect_token(TOKEN_COLON_ASSIGN);
        parse_expr();
        if (kind == TYPE_ARRAY) {
            asm_stw_var(assembler, current_reg-1, current_reg-2, sym->var_index);
        } else if (kind == TYPE_RECORD) {
            asm_stw_var(assembler, current_reg-1, current_reg-2, sym->var_index);
        } else {
            asm_stw_var(assembler, current_reg-1, R0, sym->var_index);
        }
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
    // TODO(dannas): Replace this dummy type with the correct one.
    parse_selector(type_integer);
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

// Should really be under parse_ident_list but I'm making an extra function just to ease
// the task of returning a field list for the record types.
void parse_field_ident_list(TypeField **fields, size_t *num_fields) {
    if (!is_token(TOKEN_NAME)) {
        fatal_error_here("Expected name but got '%s'", token_info());
    }
    buf_push(*fields, (TypeField){.name=token.name});
    *num_fields += 1;
    next_token();
    while (match_token(TOKEN_COMMA)) {
        if (!is_token(TOKEN_NAME)) {
            fatal_error_here("Expected name but got '%s'", token_info());
        }
        buf_push(*fields, (TypeField){.name=token.name});
        *num_fields += 1;
        next_token();
    }
}

void parse_ident_list() {
    if (!is_token(TOKEN_NAME)) {
        fatal_error_here("Expected name but got '%s'", token_info());
    }
    if (!sym_push(token.name, SYM_NONE, type_none)) {
        fatal_error_here("identifier '%s' is already defined", token.name);
    }
    next_token();
    while (match_token(TOKEN_COMMA)) {
        if (!is_token(TOKEN_NAME)) {
            fatal_error_here("Expected name but got '%s'", token_info());
        }
        if (!sym_push(token.name, SYM_NONE, type_none)) {
            fatal_error_here("identifier '%s' is already defined", token.name);
        }
        next_token();
    }
}

Type* parse_type();

Type* parse_array_type() {
    expect_keyword(ARRAY_keyword);

    if (!is_token(TOKEN_NUMBER)) {
        fatal_error_here("Expected number but got %s", token_kind_name(token.kind));
    }
    if (token.number == 0) {
        fatal_error_here("Array size must not be zero");
    }
    size_t num_elems = token.number;
    next_token();
    // TODO(dannas): Add constexpr evaluation for constants
    //parse_expr();
    expect_keyword(OF_keyword);
    Type *base = parse_type();
    return type_array(base, num_elems);
}

void parse_field_list(TypeField **fields, size_t *num_fields) {
    if (is_token(TOKEN_NAME)) {
        //parse_ident_list();
        size_t offset = buf_len(*fields);
        parse_field_ident_list(fields, num_fields);
        expect_token(TOKEN_COLON);
        Type *type = parse_type();
        size_t size = 0;

        for (TypeField *it = *fields + offset; it != buf_end(*fields); it++) {
            it->type = type;
            // TODO(dannas): Fix alignmnent. For now, just assume that all types have the same alignment
            // and that no padding is required.
            it->offset = size;
            size += type->size;
        }
    }
}

Type* parse_record_type() {
    expect_keyword(RECORD_keyword);
    Type *type = type_alloc(TYPE_RECORD);
    parse_field_list(&type->record.fields, &type->record.num_fields);
    while (match_token(TOKEN_SEMICOLON)) {
        parse_field_list(&type->record.fields, &type->record.num_fields);
    }
    for (TypeField *it = type->record.fields; it != buf_end(type->record.fields); it++) {
        type->size += it->type->size;
    }
    expect_keyword(END_keyword);
    return type;
}

Type* parse_type() {
    Type *type;
    if (is_token(TOKEN_NAME)) {
        Sym *sym = sym_get(token.name);
        if (!sym) {
            fatal_error_here("'%s' is an unknown type", token.name);
        }
        if (sym->kind != SYM_TYPE) {
            fatal_error_here("Expected '%s' to be TYPE but it was %s", token.name, sym_kind_name(sym->kind));
        }
        type = sym->type;
        next_token();
    } else if (is_keyword(ARRAY_keyword)) {
        type = parse_array_type();
    } else if (is_keyword(RECORD_keyword)) {
        type = parse_record_type();
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
    Sym *scope = sym_enter();
    size_t var_index_on_entry = var_index;

    parse_declarations();
    if (match_keyword(BEGIN_keyword)) {
        parse_stmt_sequence();
    }
    expect_keyword(END_keyword);
    expect_token(TOKEN_NAME);
    // TODO(dannas): Determine if we're at the bottom of the call stack.
    // If so, then do a RET 0
    asm_br_op(assembler, RET, 0);

    sym_leave(scope);
    var_index = var_index_on_entry;
}

void parse_proc_declaration() {
    parse_proc_heading();
    expect_token(TOKEN_SEMICOLON);
    parse_proc_body();
}

void parse_declarations() {
    if (match_keyword(CONST_keyword)) {
        while (is_token(TOKEN_NAME)) {
            if (!sym_push(token.name, SYM_CONST, type_integer)) {
                fatal_error_here("identifier '%s' is already defined", token.name);
            }
            next_token();
            expect_token(TOKEN_EQ);
            if (!is_token(TOKEN_NUMBER)) {
                fatal_error_here("Expected number or ( or ~ but got %s", token_kind_name(token.kind));
            }
            symbols_end[-1].val = token.number;
            next_token();
            // TODO(dannas): Add constexpr evaluation for constants
            //parse_expr();
            expect_token(TOKEN_SEMICOLON);
        }
    }
    if (match_keyword(TYPE_keyword)) {
        while (is_token(TOKEN_NAME)) {
            const char *name = token.name;
            next_token();
            expect_token(TOKEN_EQ);
            Type *type = parse_type();
            sym_push(name, SYM_TYPE, type);
            expect_token(TOKEN_SEMICOLON);
        }
    }
    if (match_keyword(VAR_keyword)) {
        while (is_token(TOKEN_NAME)) {
            Sym *first = sym_enter();
            parse_ident_list();
            expect_token(TOKEN_COLON);
            Type *type = parse_type();
            for (Sym *sym = first; sym < symbols_end; sym++) {
                sym->kind = SYM_VAR;
                sym->type = type;
                sym->var_index = var_index++;
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
        Sym *scope = sym_enter();
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
        sym_leave(scope);
    } else {
        fatal_error_here("Error expected name but got %s", token_kind_name(token.kind));
    }
}
