#include <iostream>
#include <string>
#include <cstdio>
#include <cassert>

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

int main(int argc, char* argv[]) {
    FILE* fp = nullptr;

    if (argc < 2)
        fp = stdin;
    else
        fp = fopen(argv[1], "r");
    Lexer lexer(fp);

    while (true) {
        Token tok = lexer.next();
        std::cout << tok.type << "\t\t" << tok.text << "\n";
        if (tok.type == END) {
            return 0;
        }
    }
}
