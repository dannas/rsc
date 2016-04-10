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
//      assembler - simple p-code assembler
//
// SYNOPSIS
//      assembler [--test] [INPUTFILE] [OUTPUTFILE]
//
// DESCRIPTION
//      Translates assembly statements found in INPUTFILE into bytecode that is
//      stored in OUTPUTFILE.
//
//      Passing the --test option, triggers a run of the internal test suite.
//
//      The syntax of the assembly and the organization of the bytecode is
//      taken from Terrence Pratt's book "Language Implel mentation Patterns".
//      The bytecode there is inspired by Pascals p-code.
//
// LIMITATIONS
//      Only 32 bit integers as operands.
//      No macro functionality.
//      Error reporting is incomplete and inconsistent.

#define FOR_EACH_TOKEN(macro) \
    macro(LABEL)              \
    macro(FUNCDEF)            \
    macro(GLOBALS)            \
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
    Lexer(FILE* fp_) : fp_(fp_), line_(0), col_(0) {
        consume();
    }
    Token next() {
        while (c_ != EOF) {
            switch (c_) {
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
                if (c_ == '-' || isdigit(c_))
                    return operand();
                if (isalpha(c_))
                    return id();
            }
            assert(false && "not reachable");
        }
        return {END, ""};
    }

private:
    void consume() {
        c_ = fgetc(fp_);
        if (c_ == '\n') {
            line_++;
            col_ = 0;
        } else {
            col_++;
        }
    }

    void ws() {
        while (c_ == ' ' || c_ == '\t' || c_ == '\r')
            consume();
    }

    void comment() {
        while (c_ != '\n')
            consume();
    }
    Token id() {
        string text;
        do {
            text += c_;
            consume();
        } while (isalpha(c_));
        return {ID, text};
    }

    Token label() {
        string text;
        do {
            text += c_;
            consume();
        } while (isalpha(c_));
        if (text == ".def")
            return {FUNCDEF, text};
        else if (text == ".globals")
            return {GLOBALS, text};
        else
            return {LABEL, text};

    }

    Token operand() {
        string text;
        do {
            text += c_;
            consume();
        } while (isdigit(c_));
        return {OPERAND, text};
    }
    FILE* fp_;
    int c_;                  // lookahead character

    int line_;               // pos of current token
    int col_;                // pos of current token
    friend class Parser;    // for access to line, col
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
        auto& l = labels_[name];
        if (!l.defined)
            l.forwardRefs.push_back(ip);
        return l.address;
    }
    void define(const std::string& name, int32_t ip, vector<int32_t> &bytecode) {
        auto l = labels_[name];
        assert(!l.defined && "label already defined");

        for (auto& ref : l.forwardRefs)
            bytecode[ref] = ip;

        l.defined = true;
        l.address = ip;
        l.forwardRefs.clear();
    }
private:
    unordered_map<string, LabelSymbol> labels_;
};


// Grammar for the assembly syntax.
//
// program              => globaldeclaration functiondeclaration*
// globalsdeclaration   => NEWLINE* '.globals'
// functiondeclaration  => NEWLINE* '.def' ID 'args' '=' OPERAND ',' 'locals' '=' OPERAND NEWLINE instr*
// labeldeclaration     => NEWLINE* LABEL
// instr                => ID NEWLINE
//                      => ID OPERAND NEWLINE
//                      => ID LABEL NEWLINE
//                      => ID OPERAND ',' OPERAND NEWLINE
//
// TODO(dannas): Parse .globals.
class Parser {
public:
    Parser(Lexer& lexer_) : lexer_(lexer_), ip_(0) {
        consume();
        program();
    }

    std::vector<int32_t> code() {
        return bytecode_;
    }
private:
    void consume() {
        tok_ = lexer_.next();
        std::cout << tok_.type << "\t\t" << tok_.text << "\n";
    }

    template <typename T>
    void die(T actual, T expected) {
        cerr << "<function>:" << lexer_.line_ << ":" << lexer_.col_
            << " expected '" << expected << "' but got '" << actual << "'\n";
        exit(1);
    }

    void match(TokenType type, const string& text = "") {
        if (type != tok_.type)
            die(type, tok_.type);
        if (text != "" && text != tok_.text)
            die(text, tok_.text);
        consume();
    }

    void program() {
        while (true) {
            if (tok_.type == END)
                return;
            else if (tok_.type == NEWLINE)
                consume();
            else if (tok_.type == LABEL)
                label();
            else if (tok_.type == FUNCDEF)
                funcdef();
            else if (tok_.type == ID)
                instr();
            else
                assert(false && "unreachable");
        }
    }
    void label() {
        symtab_.define(tok_.text, ip_, bytecode_);
        consume();
        match(NEWLINE);
    }

    void globals() {
        match(GLOBALS);
        while (tok_.type == ID)
            global();
    }
    void global() {
        int x = atoi(tok_.text.c_str());
        globals_.push_back(x);
    }

    void funcdef() {
        match(FUNCDEF);
        symtab_.define(tok_.text, ip_, bytecode_);
        consume();
        match(ID, "args");
        match(EQUALSIGN);
        int32_t operand = atoi(tok_.text.c_str());
        pushByteCode(OP_LOAD);
        pushByteCode(operand);
        consume();
        match(COMMA);
        match(ID, "locals");
        match(EQUALSIGN);
        // TODO(dannas): Store number of locals.
        match(OPERAND);
        match(NEWLINE);

        while (tok_.type == ID)
            instr();
    }

    void instr() {
        assert(instrExists(tok_.text) && "unkown opcode");
        OpCode code = opCodeForInstr(tok_.text);
        pushByteCode(code);
        consume();

        if (tok_.type == NEWLINE) {
            match(NEWLINE);
            return;
        }
        if (tok_.type == LABEL) {
            int32_t addr = symtab_.lookup(tok_.text, ip_);
            pushByteCode(addr);
            consume();
            match(NEWLINE);
            return;
        }
        if (tok_.type == OPERAND) {
            int32_t operand = atoi(tok_.text.c_str());
            pushByteCode(operand);
            consume();
        }

        if (tok_.type == OPERAND) {
            int32_t operand = atoi(tok_.text.c_str());
            pushByteCode(operand);
            match(NEWLINE);
            return;
        }

        match(NEWLINE);
        return;
    }

    void pushByteCode(int32_t code) {
        bytecode_.push_back(code);
        ip_++;
    }
    Lexer& lexer_;
    Token tok_;                  // lookahead token
    vector<int32_t> bytecode_;   // the instruction output
    vector<int32_t> globals_;    // the globals output
    int ip_;                     // current position in |bytecode|
    SymbolTable symtab_;         // map labels and func defs to addresses
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

void disassemble(const char* fname) {
    FILE* fp = fopen(fname, "r");
    assert(fp);

    int32_t c;
    while (true) {

        fread(&c, sizeof(c), 1, fp);
        if (feof(fp))
            return;
        cerr << "\tc=" << c << "\n";
        switch(c) {
        // TODO(dannas): Clean up this mess
#define macro(op, desc, nargs) \
        case op: \
            cout << desc; \
            if (nargs) { \
                fread(&c, sizeof(1), 1, fp); \
                cout << " " << c; \
            } \
            cout << "\n"; \
            break;
        FOR_EACH_OPCODE(macro);
#undef macro
        default:
        assert(false && "uncreachable");
        }
    }
    fclose(fp);
}

int main(int argc, char* argv[]) {
    string op = argc >= 2 ? argv[1] : "";

    if (op == "test") {
        runtests();
    } else if (op == "disassemble") {
        disassemble(argv[2]);
    } else if (op == "assemble") {

        if (argc != 4) {
            cerr << "usage: " << argv[0]  << " assemble INFILE OUTFILE\n";
            exit(1);
        }
        FILE* fp = fopen(argv[2], "r");
        FILE* outfp = fopen(argv[3], "w");

        assert(fp);
        assert(outfp);
        Lexer lexer(fp);
        Parser parser(lexer);
        auto code = parser.code();

        fwrite(code.data(), sizeof(int32_t), code.size(), outfp);

        fclose(fp);
        fclose(outfp);
    } else {
        cerr << "unknown operation\n";
        exit(1);
    }
}
