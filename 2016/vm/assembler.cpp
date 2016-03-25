#include <iostream>
#include <string>
#include <cstdio>
#include <cassert>

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
// TODO(dannas): Write bytecode.
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
        match(LABEL);
        match(NEWLINE);
    }
    void funcdef() {
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
        match(ID);
        if (tok.type == NEWLINE) {
            match(NEWLINE);
            return;
        }
        if (tok.type == LABEL) {
            match(LABEL);
            match(NEWLINE);
            return;
        }
        if (tok.type == OPERAND) {
            match(OPERAND);
            if (tok.type == OPERAND) {
                match(OPERAND);
                match(NEWLINE);
                return;
            } else {
                match(NEWLINE);
                return;
            }
        }
        assert(false && "unreachable");
    }
    Lexer& lexer;
    Token tok;  // lookahead token
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
