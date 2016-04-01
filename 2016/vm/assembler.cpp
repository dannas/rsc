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
    Lexer(FILE* fp_) : fp(fp_) {
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
    }

    void ws() {
        while (c == ' ' || c == '\t' || c == '\r')
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
};

// TODO(dannas): Clean up the usage of LabelSymbol.
// At the moment, we destroy the forwardrefs when we insert a new LabelSymbol
// into the map in |label()|.
struct LabelSymbol {
    LabelSymbol() : defined(false), address(0), forwardRefs() {}
    LabelSymbol(int32_t addr) : defined(true), address(addr), forwardRefs() {}
    void addForwardRef(int32_t ref) {
        forwardRefs.push_back(ref);
    }

    bool resolved() {
        return forwardRefs.empty();
    }

    bool defined;
    int32_t address;
    vector <int32_t> forwardRefs;
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
    void match(TokenType expected) {
        assert(expected == tok.type);
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
        }

    }
    void label() {
        auto i = labels.find(tok.text);
        assert(i == end(labels) || i->second.defined == false);
        for (const auto& pos : i->second.forwardRefs)
            bytecode[pos] = ip;
        labels[tok.text] = LabelSymbol(ip);
        consume();
        match(NEWLINE);
    }
    void funcdef() {
        // TODO: Write func to symbol table.
        match(FUNCDEF);
        match(ID);
        match(ID);
        match(EQUALSIGN);
        match(OPERAND);
        match(COMMA);
        match(ID);
        match(EQUALSIGN);
        match(OPERAND);
        match(NEWLINE);
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
            if (labels.find(tok.text) == end(labels)) {
                auto ls = LabelSymbol();
                ls.addForwardRef(ip);
                labels[tok.text] = ls;
                pushByteCode(0);
            } else {
                pushByteCode(labels[tok.text].address);
            }

            // TODO: lookup label in symboltable
            match(LABEL);
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
    unordered_map<string, LabelSymbol> labels;
};

void testAdd() {
    char buf[] =
        "iconst 1"   "\n"
        "iconst 2"   "\n"
        "iadd"       "\n"
        "print"      "\n"
        "halt"       "\n";

    FILE* fp = fmemopen(buf, strlen(buf), "r");
    assert(fp);
    Lexer lexer(fp);
    Parser parser(lexer);
    auto code = parser.code();
    assert(code.size() == 7);
    assert(code[0] == OP_ICONST);
    assert(code[1] == 1);
    assert(code[2] == OP_ICONST);
    assert(code[3] == 2);
    assert(code[4] == OP_IADD);
    assert(code[5] == OP_PRINT);
    assert(code[6] == OP_HALT);
    fclose(fp);
}

void testJump() {
    char buf[] =
        "iconst 1"   "\n"
        "brt .end"   "\n"
        ".end"       "\n"
        "halt"       "\n";;

    FILE* fp = fmemopen(buf, strlen(buf), "r");
    assert(fp);
    Lexer lexer(fp);
    Parser parser(lexer);
    auto code = parser.code();
    assert(code.size() == 5);
    assert(code[0] == OP_ICONST);
    assert(code[1] == 1);
    assert(code[2] == OP_BRT);
    assert(code[3] == 4);
    assert(code[4] == OP_HALT);
    fclose(fp);
}

void runtests() {
    testAdd();
    testJump();
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
