#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <stdexcept>

#include "vm.h"

using namespace std;

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

// A LL(1) recursive-decent Lexer.
class Lexer {
public:
    Lexer(istream& in) : in_(in), line_(0), col_(0) {
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
        c_ = in_.get();
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
    istream& in_;
    int c_;                  // lookahead character

    int line_;               // pos of current token
    int col_;                // pos of current token
    friend class Parser;     // for access to line, col
};

struct LabelSymbol {
    LabelSymbol()
        : defined(false)
        , address(0)
        , forwardRefs()
        , nargs(-1)
        , nlocals(-1) {}

    bool defined;                   // has definition been seen?
    int32_t address;                // address of the label
    vector <int32_t> forwardRefs;   // offsets into the bytecode
    int32_t nargs;                  // number of parameters for funcdef
    int32_t nlocals;                // number of locals for funcdef
};



// SymbolTable for a monolithic scope.
//
// When a definition is found, record the (symbol, ip) in the table.
// When the symbol is referenced, replace it with ip from the table.
//
// But symbols can be referenced before they're defined. We solve that
// by keeping track of those forward references and backpatch them upon
// encountering the symbol definition.
class SymbolTable {
public:
    SymbolTable() {
    }
    LabelSymbol lookup(const string& name, int32_t ip) {
        auto& l = labels_[name];
        if (!l.defined)
            l.forwardRefs.push_back(ip);
        return l;
    }

    void defineLabel(const string& name, int32_t ip,
                     vector<int32_t> &bytecode) {
        auto& l = labels_[name];
        assert(!l.defined && "label already defined");

        // Backpatch previous references to the symbol.
        for (auto& ref : l.forwardRefs)
            bytecode[ref] = ip;

        l.defined = true;
        l.address = ip;
        l.forwardRefs.clear();
    }

    void defineFunction(const string& name, int32_t ip,
                        int32_t nargs, int32_t nlocals,
                        vector<int32_t> &bytecode) {
        auto& l = labels_[name];
        assert(!l.defined && "label already defined");

        // Backpatch previous references to the symbol.
        for (auto& ref : l.forwardRefs) {
            bytecode[ref] = ip;
            bytecode[ref+1] = nargs;
            bytecode[ref+2] = nlocals;
        }

        l.defined = true;
        l.address = ip;
        l.nargs = nargs;
        l.forwardRefs.clear();
    }
private:
    unordered_map<string, LabelSymbol> labels_;
};

// A LL(1) recursive-decent Parser.
//
// See vm.h for a description of the grammar.
//
class Parser {
public:
    Parser(Lexer& lexer_) : lexer_(lexer_), ip_(0) {
        consume();
        program();
    }

    vector<int32_t> code() {
        return bytecode_;
    }
private:
    void consume() {
        tok_ = lexer_.next();
        cout << tok_.type << "\t\t" << tok_.text << "\n";
    }


    template <typename T>
    void die(T actual, T expected) {
        stringstream ss;
        ss << "function>:" << lexer_.line_ << ":" << lexer_.col_
            << " expected '" << expected << "' but got '" << actual << "'\n";
        throw invalid_argument(ss.str());
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
        symtab_.defineLabel(tok_.text, ip_, bytecode_);
        pushByteCode(OP_LABEL);
        consume();
        match(NEWLINE);
    }

    void funcdef() {
        match(FUNCDEF);
        string text = tok_.text;
        int32_t ip = ip_;
        consume();
        match(ID, "args");
        match(EQUALSIGN);
        int32_t nargs = atoi(tok_.text.c_str());
        consume();
        match(COMMA);
        match(ID, "locals");
        match(EQUALSIGN);
        int32_t nlocals = atoi(tok_.text.c_str());
        match(OPERAND);
        match(NEWLINE);
        symtab_.defineFunction(text, ip, nargs, nlocals, bytecode_);

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
            auto l = symtab_.lookup(tok_.text, ip_);
            pushByteCode(l.address);
            consume();

            // if the symbol is a reference to a function
            // insert nargs and nlocals as well
            if (code == OP_CALL) {
                pushByteCode(l.nargs);
                pushByteCode(l.nlocals);
            }
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

    void callInstr() {
    }

    void pushByteCode(int32_t code) {
        bytecode_.push_back(code);
        ip_++;
    }
    Lexer& lexer_;
    Token tok_;                  // lookahead token
    vector<int32_t> bytecode_;   // the instruction output
    int ip_;                     // current position in |bytecode|
    SymbolTable symtab_;         // map labels and func defs to addresses
};

vector<int32_t> assemble(istream& in) {
    Lexer lexer(in);
    Parser parser(lexer);
    return parser.code();
}

static int32_t readInt(istream& in) {
    int32_t val;
    in.read(reinterpret_cast<char*>(&val), sizeof(val));
    return val;
}

string disassemble(istream& in) {

    int32_t opcode;
    stringstream ss;

    while (true) {
        opcode = readInt(in);
        if (!in)
            goto out;

        switch(opcode) {
#define macro(op, desc, nargs)                  \
        case op:                                \
            ss << desc;                         \
            for (int i = 0; i < nargs; ++i) {   \
                ss << " " << readInt(in);       \
            }                                   \
            ss << "\n";                         \
            break;
        FOR_EACH_OPCODE(macro);
#undef macro
        default:
        assert(false && "uncreachable");
        }
    }

out:
    return ss.str();
}
