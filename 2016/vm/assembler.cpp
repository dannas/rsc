#include <iostream>
#include <string>
#include <cstdio>
#include <cassert>
#include <vector>
#include <cstdlib>

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
    Parser(Lexer& lexer_) : lexer(lexer_) {
        tok = lexer.next();
        program();
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
        // TODO: Write label to symbol table.
        match(LABEL);
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
        bytecode.push_back(code);
        consume();

        if (tok.type == NEWLINE) {
            match(NEWLINE);
            return;
        }
        if (tok.type == LABEL) {
            // TODO: lookup label in symboltable
            match(LABEL);
            match(NEWLINE);
            return;
        }
        if (tok.type == OPERAND) {
            int32_t operand = atoi(tok.text.c_str());
            bytecode.push_back(operand);
            consume();
        }

        if (tok.type == OPERAND) {
            int32_t operand = atoi(tok.text.c_str());
            bytecode.push_back(operand);
            match(NEWLINE);
            return;
        }

        match(NEWLINE);
        return;
    }
    Lexer& lexer;
    Token tok;  // lookahead token
    vector<int32_t> bytecode;
    int line;
};

int main(int argc, char* argv[]) {
    FILE* fp = nullptr;

    if (argc < 2)
        fp = stdin;
    else
        fp = fopen(argv[1], "r");
    Lexer lexer(fp);
    Parser parser(lexer);
}
