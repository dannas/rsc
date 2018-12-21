
// Vocabulary for the Oberon lexer
//
//      * DIV MOD & + - OR
//      = # < <= > >= . , : ) ]
//      OF THEN DO ( [ ~ := ;
//      END ELSE ELSIF IF WHILE
//      ARRAY RECORD CONST TYPE VAR PROCEDURE BEGIN MODULE
//

typedef enum TokenKind {
    TOKEN_EOF,
    TOKEN_ADD,
    TOKEN_MINUS,
    TOKEN_AND,
    TOKEN_MULTIPLY,
    TOKEN_NUMBER,
    TOKEN_IDENTIFIER,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_EQ,
    TOKEN_LE,
    TOKEN_LT,
    TOKEN_GT,
    TOKEN_GE,
    TOKEN_NE,
    TOKEN_DOT,
    TOKEN_COMMA,
    TOKEN_SEMICOLON,
    TOKEN_NOT,
    TOKEN_COLON,
    TOKEN_COLON_ASSIGN,
    TOKEN_KEYWORD,
    TOKEN_NAME,
    TOKEN_DIV,
    TOKEN_MOD,
    TOKEN_OR,
} TokenKind;

typedef struct SrcPos {
    const char *name;
    int line;
} SrcPos;

typedef struct Token {
    TokenKind kind;
    const char *start;
    const char *end;
    SrcPos pos;
    union {
        uint32_t number;
        const char *str_val;
        const char *name;
    };
} Token;

Token token;
const char *stream;

void error(SrcPos pos, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf("%s(%d): error: ", pos.name, pos.line);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
}

#define error_here(...) (error(token.pos, __VA_ARGS__))
#define fatal_error(...) (error(__VA_ARGS__), exit(1))
#define fatal_error_here(...) (error(token.pos, __VA_ARGS__), exit(1))

const char* DIV_keyword;
const char* MOD_keyword;
const char* OR_keyword;
const char* OF_keyword;
const char* THEN_keyword;
const char* DO_keyword;
const char* END_keyword;
const char* ELSE_keyword;
const char* ELSEIF_keyword;
const char* IF_keyword;
const char* WHILE_keyword;
const char* ARRAY_keyword;
const char* RECORD_keyword;
const char* CONST_keyword;
const char* TYPE_keyword;
const char* VAR_keyword;
const char* PROCEDURE_keyword;
const char* BEGIN_keyword;
const char* MODULE_keyword;

const char** keywords;
const char* first_keyword;
const char* last_keyword;

#define KEYWORD(name) name##_keyword = str_intern(#name); buf_push(keywords, name##_keyword)

void init_keywords() {
    static bool inited;
    if (inited) {
        return;
    }
    KEYWORD(DIV);
    KEYWORD(MOD);
    KEYWORD(OR);
    KEYWORD(OF);
    KEYWORD(THEN);
    KEYWORD(DO);

    KEYWORD(END);
    KEYWORD(ELSE);
    KEYWORD(ELSEIF);
    KEYWORD(IF);
    KEYWORD(WHILE);

    KEYWORD(ARRAY);
    KEYWORD(RECORD);
    KEYWORD(CONST);
    KEYWORD(TYPE);
    KEYWORD(VAR);
    KEYWORD(PROCEDURE);
    KEYWORD(BEGIN);
    KEYWORD(MODULE);

    const char *arena_end = str_arena.end;

    first_keyword = DIV_keyword;
    last_keyword = MODULE_keyword;

    assert(arena_end == str_arena.end);
    inited = true;
}

#undef KEYWORD

bool is_keyword_name(const char *name) {
    return name >= first_keyword && name <= last_keyword;
}

#define CASE1(ch, token_kind) \
    case ch: \
        token.kind = token_kind; \
        stream++; \
        break;
#define CASE2(ch1, kind1, ch2, kind2) \
    case ch1: \
        stream++; \
        if (*stream == ch2) { \
            token.kind = kind2; \
            stream++; \
        } else { \
            token.kind = kind1; \
        } \
        break;

bool is_keyword(const char *name);

void next_token(void) {
repeat:
    token.start = stream;
    switch (*stream) {
    case ' ':
    case '\t':
    case '\n':
    case '\r':
        if (*stream == '\n') {
            token.pos.line++;
        }
        stream++;
        goto repeat;
    case '(':
        token.kind = TOKEN_LPAREN;
        stream++;
        if (*stream == '*') {
            stream++;
            int level = 1;
            while (*stream && level) {
                if (stream[0] == '(' && stream[1] == '*') {
                    stream += 2;
                    level++;
                } else if (stream[0] == '*' && stream[1] == ')') {
                    stream += 2;
                    level--;
                } else {
                    if (stream[0] == '\n') {
                        token.pos.line++;
                    }
                    stream++;
                }
            }
            goto repeat;
        }
        break;
    case '0'...'9': {
        uint64_t val = 0;
        while (isdigit(*stream)) {
            val = 10 * val + (*stream - '0');
            if (val > UINT32_MAX) {
                error_here("Integer literal overflow");
                while (isdigit(*stream)) {
                    stream++;
                }
                val = 0;
                break;
            }
            stream++;
        }
        token.kind = TOKEN_NUMBER;
        token.number = val;
        break;
    }
    CASE1('+', TOKEN_ADD)
    CASE1('-', TOKEN_MINUS)
    CASE1('*', TOKEN_MULTIPLY)
    CASE1('&', TOKEN_AND)
    CASE1(')', TOKEN_RPAREN)
    CASE1('[', TOKEN_LBRACKET)
    CASE1(']', TOKEN_RBRACKET)
    CASE1('=', TOKEN_EQ)
    CASE1('#', TOKEN_NE)
    CASE1('.', TOKEN_DOT)
    CASE1(',', TOKEN_COMMA)
    CASE1(';', TOKEN_SEMICOLON)
    CASE1('~', TOKEN_NOT)
    CASE2('<', TOKEN_LT, '=', TOKEN_LE)
    CASE2('>', TOKEN_GT, '=', TOKEN_GE)
    CASE2(':', TOKEN_COLON, '=', TOKEN_COLON_ASSIGN)
    case 'a'...'z':
    case 'A'...'Z':
    case '_':
        while (isalnum(*stream) || *stream == '_') {
            stream++;
        }
        token.name = str_intern_range(token.start, stream);
        token.kind = is_keyword_name(token.name) ? TOKEN_KEYWORD : TOKEN_NAME;

        // Simplify expr parsing by letting each operator have a unique token kind.
        if (is_keyword(DIV_keyword)) {
            token.kind = TOKEN_DIV;
        } else if (is_keyword(MOD_keyword)) {
            token.kind = TOKEN_MOD;
        } else if (is_keyword(OR_keyword)) {
            token.kind = TOKEN_OR;
        }
        break;
    case '\0':
        token.kind = TOKEN_EOF;
        break;
    default:
        error_here("Invalid '%c' token, skipping", *stream);
        stream++;
        goto repeat;
    }
}


char* token_kind_names[] = {
    [TOKEN_EOF] = "EOF",
    [TOKEN_ADD] = "+",
    [TOKEN_MINUS] = "-",
    [TOKEN_MULTIPLY] = "*",
    [TOKEN_AND] = "&",
    [TOKEN_LPAREN] = "(",
    [TOKEN_RPAREN] = ")",
    [TOKEN_LBRACKET] = "[",
    [TOKEN_RBRACKET] = "]",
    [TOKEN_EQ] = "=",
    [TOKEN_LT] = "<",
    [TOKEN_LE] = "<=",
    [TOKEN_GT] = ">",
    [TOKEN_GE] = ">=",
    [TOKEN_NE] = "#",
    [TOKEN_DOT] = ".",
    [TOKEN_COMMA] = ",",
    [TOKEN_SEMICOLON] = ";",
    [TOKEN_NOT] = "~",
    [TOKEN_COLON] = ":",
    [TOKEN_COLON_ASSIGN] = ":=",
    [TOKEN_NUMBER] = "number",
    [TOKEN_IDENTIFIER] = "identifier",
    [TOKEN_KEYWORD] = "keyword",
    [TOKEN_NAME] = "name",
    [TOKEN_DIV] = "DIV",
    [TOKEN_MOD] = "MOD",
    [TOKEN_OR] = "OR",
};

char* token_kind_name(TokenKind kind) {
    if (kind < sizeof(token_kind_names)/sizeof(token_kind_names[0])) {
        return token_kind_names[kind];
    }
    return "unknown";
}

const char* token_info() {
    if (token.kind == TOKEN_KEYWORD || token.kind == TOKEN_NAME) {
        return token.name;
    }
    return token_kind_name(token.kind);
}

bool is_token(TokenKind kind) {
    return token.kind == kind;
}

bool match_token(TokenKind kind) {
    if (is_token(kind)) {
        next_token();
        return true;
    }
    return false;
}

bool expect_token(TokenKind kind) {
    if (is_token(kind)) {
        next_token();
        return true;
    }
    fatal_error_here("Expected token %s but got %s", token_kind_name(kind), token_info());

    return false;
}

bool expect_keyword(const char *name) {
    if (is_keyword(name)) {
        next_token();
        return true;
    }
    fatal_error_here("Expected keyword %s but got %s", name, token_info());

    return false;
}

bool is_keyword(const char *name) {
    return token.kind == TOKEN_KEYWORD && token.name == name;
}

bool match_keyword(const char *name) {
    if (is_keyword(name)) {
        next_token();
        return true;
    }
    return false;
}

void init_stream(const char *name, const char *buf) {
    stream = buf;
    init_keywords();
    token.pos.name = name ? name : "<string>";
    token.pos.line = 1;
    next_token();
}
