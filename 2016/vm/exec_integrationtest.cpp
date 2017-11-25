
#include "vm.h"

#include <cstdint>
#include <vector>

#include <gtest/gtest.h>

using namespace std;

#define EXEC_AND_COMPARE(buf, expected) \
    do { \
        stringstream as(buf); \
        auto code = assemble(as); \
        stringstream ss; \
        interpret(code, ss); \
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

TEST(Integration, addFunction) {
    char buf[] = R"(
        iconst 1
        iconst 2
        call .f
        print
        halt
        .def .f args=2, locals=0
        load 0
        load 1
        iadd
        ret
    )";
    string expected = "3\n";

    EXEC_AND_COMPARE(buf, expected);
}

TEST(Integration, pow) {
    char buf[] = R"(
         iconst 2
         iconst 2
         call .pow
         print
         halt

        .def .pow args=2, locals=0  ; .pow x, n
            load 0                  ; product =
        .start
            load 1                  ; if n == 1 goto .end
            iconst 1
            ieq
            brt .end
            pop

            load 0                  ; product = product * x
            imult

            load 1                  ; n--
            iconst 1
            isub
            store 1
            br .start

        .end                        ; return product
            pop
            ret

    )";

    string expected = "4\n";

    EXEC_AND_COMPARE(buf, expected);
}

TEST(Integration, gcd) {
    char buf[] = R"(
    iconst 25484848
    iconst 1448
    call .gcd
    print
    halt

    .def .gcd args=2, locals=1   ; .gcd x, y
        load 0
        load 1
        imod
        store 2                  ; r = x % y
    .start
        load 2
        iconst 0
        ieq
        brt .end                ; if r == 0 goto .end
        pop
        load 1
        store 0                 ; x = y

        load 2
        store 1                 ; y = r

        load 0
        load 1
        imod
        store 2                 ; r = x % y

        br .start
    .end
        pop
        load 1
        ret                     ; return y
    )";
    string expected = "8\n";

    EXEC_AND_COMPARE(buf, expected);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
