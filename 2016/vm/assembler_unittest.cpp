#include "vm.h"

#include <cstdint>
#include <vector>
#include <algorithm>

#include <gtest/gtest.h>

using namespace std;

#define ASSEMBLE_AND_COMPARE(buf, expected) \
    do { \
        stringstream ss(buf); \
        auto actual = assemble(ss); \
        ASSERT_EQ(expected, actual); \
    } while (0)


TEST(Assembler, add) {
    char buf[] = R"(
        iconst 1
        iconst 2
        iadd
        print
        halt
    )";

    vector<int32_t> expected = {
        OP_ICONST, 1,
        OP_ICONST, 2,
        OP_IADD,
        OP_PRINT,
        OP_HALT
    };

    ASSEMBLE_AND_COMPARE(buf, expected);
}

TEST(Assembler, jump) {
    char buf[] = R"(
        iconst 1
        brt .end
        .end
        halt
    )";

    vector<int32_t> expected = {
        OP_ICONST, 1,
        OP_BRT, 5,
        OP_LABEL,
        OP_HALT
    };

    ASSEMBLE_AND_COMPARE(buf, expected);
}

TEST(Assembler, function) {
    char buf[] = R"(
        call .f
        halt
        .def .f args=0, locals=0
        ret
    )";

    vector<int32_t> expected = {
        OP_CALL, 5, 0, 0,
        OP_HALT,
        OP_RET,
    };

    ASSEMBLE_AND_COMPARE(buf, expected);
}

TEST(Assembler, comment) {
    char buf[] = R"(
        ; The first comment
        halt ; The second  comment
        ; The third comment
    )";
    vector<int32_t> expected = {
        OP_HALT,
    };

    ASSEMBLE_AND_COMPARE(buf, expected);
}


template <typename T>
string trim(T b, T e) {

    while (b != e && isspace(*b)) {
        b++;
    }

    while (b != e && isspace(*(e-1))) {
        e--;
    }
    return string(b, e);
}

// Returns trimmed lines. Empty lines are stripped.
string trimLines(const string& lines) {
    string trimmed;
    auto b = begin(lines);
    auto eos = end(lines);

    auto e = find(b, eos, '\n');

    while (e != eos) {
        auto s = trim(b, e);
        if (!s.empty())
            trimmed += s + '\n';
        b = e + 1;
        e = find(b, eos, '\n');
    }

    trimmed += trim(b, e);
    return trimmed;
}

#define DISASSEMBLE_AND_COMPARE(buf, expected) \
    do { \
        stringstream ss; \
        ss.write(reinterpret_cast<char*>(buf.data()), buf.size() * sizeof(buf[0])); \
        ss.seekg(0); \
        auto actual = disassemble(ss); \
        auto trimmedExpected = trimLines(expected); \
        ASSERT_EQ(trimmedExpected, actual); \
    } while (0)


TEST(Disassembler, add) {
    vector<int32_t> buf = {
        OP_ICONST, 1,
        OP_ICONST, 2,
        OP_IADD,
        OP_PRINT,
        OP_HALT
    };

    string expected = R"(
        iconst 1
        iconst 2
        iadd
        print
        halt
    )";
    DISASSEMBLE_AND_COMPARE(buf, expected);
}

TEST(Disassembler, addFunction) {
    vector<int32_t> buf = {
        OP_ICONST, 1,
        OP_ICONST, 2,
        OP_CALL, 7, 2, 1,
        OP_PRINT,
        OP_HALT,
        OP_LOAD, 0,
        OP_LOAD, 1,
        OP_IADD,
        OP_STORE, 2,    // just for exercising the store instruction
        OP_LOAD,  2,
        OP_RET
    };
    string expected = R"(
        iconst 1
        iconst 2
        call 7 2 1
        print
        halt
        load 0
        load 1
        iadd
        store 2
        load 2
        ret
    )";

    DISASSEMBLE_AND_COMPARE(buf, expected);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
