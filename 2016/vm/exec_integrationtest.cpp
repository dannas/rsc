
#include "vm.h"

#include <cstdint>
#include <vector>

#include <gtest/gtest.h>

using namespace std;

#define EXEC_AND_COMPARE(buf, expected) \
    do { \
        stringstream as(buf); \
        auto code = assemble(as); \
        int32_t globals[] = {}; \
        stringstream ss; \
        interpret(code.data(), globals, ss); \
        ss.seekg(0); \
        string actual = ss.str(); \
        ASSERT_EQ(expected, actual); \
    } while (0)

TEST(Integration, simplePrint) {
    char buf[] = R"(
        iconst 1
        iconst 2
        iadd
        print
        halt
    )";
    string expected = "3\n";

    EXEC_AND_COMPARE(buf, expected);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
