#include <iostream>
#include <string>
#include <cstdio>
#include <cassert>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <unordered_map>

#include "opcodes.h"

using namespace std;

// NAME
//      assembler - TODO
//
// SYNOPSIS
//      TODO
//
// DESCRIPTION
//      TODO
//
// LIMITATIONS
//      TODO

#define FOR_EACH_TOKEN(macro) \
    macro(LABEL)              \
    macro(FUNCDEF)            \
    macro(COMMA)              \
    macro(EQUALSIGN)          \
    macro(NEWLINE)            \
    macro(ID)                 \
    macro(OPERAND)            \
    macro(END)                \

enum TokenType {
#define macro(x) x,
    FOR_EACH_TOKEN(macro)
#undef macro
};

ostream& operator<< (ostream& os, TokenType t) {
    switch (t) {
#define macro(x) case x: os << #x; break;
    FOR_EACH_TOKEN(macro)
#undef macro
    default:
        assert(false && "unreachable");
    }
    return os;
}

struct Token {
    TokenType type;
    string text;
};

class Lexer {
public:
    Lexer(FILE* fp_) : fp(fp_), line(0), col(0) {
        consume();
    }
    Token next() {
        while (c != EOF) {
            switch (c) {
            case ' ':
            case '\t':
            case '\r':
                ws();
                continue;
            case ';':
                comment();
                continue;
            case '\n':
                consume();
                return {NEWLINE, "\\n"};
            case ',':
                consume();
                return {COMMA, ","};
            case '=':
                consume();
                return {EQUALSIGN, "="};
            case '.':
                return label();
            case EOF:
                return {END, ""};
            default:
                if (c == '-' || isdigit(c))
                    return operand();
                if (isalpha(c))
                    return id();
            }
            assert(false && "not reachable");
        }
        return {END, ""};
    }

private:
    void consume() {
        c = fgetc(fp);
        if (c == '\n') {
            line++;
            col = 0;
        } else {
            col++;
        }
    }

    void ws() {
        while (c == ' ' || c == '\t' || c == '\r')
            consume();
    }

    void comment() {
        while (c != '\n')
            consume();
    }
    Token id() {
        string text;
        do {
            text += c;
            consume();
        } while (isalpha(c));
        return {ID, text};
    }

    Token label() {
        string text;
        do {
            text += c;
            consume();
        } while (isalpha(c));
        if (text == ".def")
            return {FUNCDEF, text};
        else
            return {LABEL, text};

    }

    Token operand() {
        string text;
        do {
            text += c;
            consume();
        } while (isdigit(c));
        return {OPERAND, text};
    }
    FILE* fp;
    int c;

    int line;
    int col;
    friend class Parser;
};

struct LabelSymbol {
    LabelSymbol() : defined(false), address(0),    forwardRefs() {}

    bool defined;
    int32_t address;
    vector <int32_t> forwardRefs;
};

class SymbolTable {
public:
    SymbolTable() {
    }
    int32_t lookup(const std::string& name, int32_t ip) {
        auto& l = labels[name];
        if (!l.defined)
            l.forwardRefs.push_back(ip);
        return l.address;
    }
    void define(const std::string& name, int32_t ip, vector<int32_t> &bytecode) {
        auto l = labels[name];
        assert(!l.defined && "label already defined");

        for (auto& ref : l.forwardRefs)
            bytecode[ref] = ip;

        l.defined = true;
        l.address = ip;
        l.forwardRefs.clear();
    }
private:
    unordered_map<string, LabelSymbol> labels;
};


// Grammar for the assembly syntax.
//
// globalsdeclaration   => NEWLINE* '.globals'
// functiondeclaration  => NEWLINE* '.def' ID 'args' '=' OPERAND ',' 'locals' '=' OPERAND
// labeldeclaration     => NEWLINE* LABEL
// instr                => ID NEWLINE
//                      => ID OPERAND NEWLINE
//                      => ID LABEL NEWLINE
//                      => ID OPERAND ',' OPERAND NEWLINE
//
// TODO(dannas): Parse .globals.
// TODO(dannas): Patch labels.
class Parser {
public:
    Parser(Lexer& lexer_) : lexer(lexer_), ip(0) {
        consume();
        program();
    }

    std::vector<int32_t> code() {
        return bytecode;
    }
private:
    void consume() {
        tok = lexer.next();
        std::cout << tok.type << "\t\t" << tok.text << "\n";
        if (tok.type == NEWLINE)
            line++;
    }

    template <typename T>
    void die(T actual, T expected) {
        cerr << "<function>:" << lexer.line << ":" << lexer.col
            << " expected '" << expected << "' but got '" << actual << "'\n";
        exit(1);
    }

    void match(TokenType type, const string& text = "") {
        if (type != tok.type)
            die(type, tok.type);
        if (text != "" && text != tok.text)
            die(text, tok.text);
        consume();
    }

    void program() {
        while (true) {
            if (tok.type == END)
                return;
            else if (tok.type == NEWLINE)
                consume();
            else if (tok.type == LABEL)
                label();
            else if (tok.type == FUNCDEF)
                funcdef();
            else if (tok.type == ID)
                instr();
            else
                assert(false && "unreachable");
        }
    }
    void label() {
        symtab.define(tok.text, ip, bytecode);
        consume();
        match(NEWLINE);
    }
    void funcdef() {
        match(FUNCDEF);
        symtab.define(tok.text, ip, bytecode);
        consume();
        match(ID, "args");
        match(EQUALSIGN);
        int32_t operand = atoi(tok.text.c_str());
        pushByteCode(OP_LOAD);
        pushByteCode(operand);
        consume();
        match(COMMA);
        match(ID, "locals");
        match(EQUALSIGN);
        match(OPERAND);
        match(NEWLINE);

        while (tok.type == ID)
            instr();
    }

    void instr() {
        assert(InstrExists(tok.text) && "unkown opcode");
        OpCode code = OpCodeForInstr(tok.text);
        pushByteCode(code);
        consume();

        if (tok.type == NEWLINE) {
            match(NEWLINE);
            return;
        }
        if (tok.type == LABEL) {
            int32_t addr = symtab.lookup(tok.text, ip);
            pushByteCode(addr);
            consume();
            match(NEWLINE);
            return;
        }
        if (tok.type == OPERAND) {
            int32_t operand = atoi(tok.text.c_str());
            pushByteCode(operand);
            consume();
        }

        if (tok.type == OPERAND) {
            int32_t operand = atoi(tok.text.c_str());
            pushByteCode(operand);
            match(NEWLINE);
            return;
        }

        match(NEWLINE);
        return;
    }

    void pushByteCode(int32_t code) {
        bytecode.push_back(code);
        ip++;
    }
    Lexer& lexer;
    Token tok;  // lookahead token
    vector<int32_t> bytecode;
    int ip;
    int line;
    SymbolTable symtab;
};

template <int N>
void checkCodeGen(char* buf, const int32_t (&expected)[N]) {
    FILE* fp = fmemopen(buf, strlen(buf), "r");
    assert(fp);
    Lexer lexer(fp);
    Parser parser(lexer);
    auto code = parser.code();

    assert(code.size() == N);

    for (size_t i = 0; i < code.size(); i++)
        assert(expected[i] == code[i]);
    fclose(fp);
}

void testAdd() {
    char buf[] =
        "iconst 1"   "\n"
        "iconst 2"   "\n"
        "iadd"       "\n"
        "print"      "\n"
        "halt"       "\n";

    int32_t code[] = {
        OP_ICONST, 1,
        OP_ICONST, 2,
        OP_IADD,
        OP_PRINT,
        OP_HALT
    };
    checkCodeGen(buf, code);
}

void testJump() {
    char buf[] =
        "iconst 1"   "\n"
        "brt .end"   "\n"
        ".end"       "\n"
        "halt"       "\n";

    int32_t code[] = {
        OP_ICONST, 1,
        OP_BRT, 4,
        OP_HALT
    };

    checkCodeGen(buf, code);
}

void testFunction() {
    char buf[] =
        "call .f"                  "\n"
        "halt"                     "\n"
        ".def .f args=0, locals=0" "\n"
        "ret"                      "\n";

    int32_t code[] = {
        OP_CALL, 3,
        OP_HALT,
        OP_LOAD, 0,
        OP_RET,
    };
    checkCodeGen(buf, code);
}

void testComment() {
    char buf[] =
        "halt ; This is a comment" "\n";
    int32_t code[] = {
        OP_HALT,
    };
    checkCodeGen(buf, code);
}

void runtests() {
    testAdd();
    testJump();
    testFunction();
    testComment();
}

int main(int argc, char* argv[]) {
    FILE* fp = nullptr;

    if (argc == 2 && strcmp(argv[1], "--test") == 0) {
        runtests();
        return 0;
    }

    if (argc < 2) {
        fp = stdin;
    } else
        fp = fopen(argv[1], "r");
    assert(fp);
    Lexer lexer(fp);
    Parser parser(lexer);
    fclose(fp);
}
