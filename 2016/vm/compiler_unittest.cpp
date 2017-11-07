#include "vm.h"

#include <cstdint>
#include <sys/mman.h>
#include <cstring>
#include <cstdio>

#include <gtest/gtest.h>

using namespace std;

TEST(Compiler, AddTwoConstants) {
    char buf[] = R"(
        iconst 3
        iconst 5
        iadd
        ret
        halt
    )";
    stringstream as(buf);
    auto bytecode = assemble(as);
    auto code = compile(bytecode);
    // rwx protection per page boundary, hence can't use malloc allocated memory
    // which may span pages in unpredictable ways.
    // TODO(dannas): Introduce W^X via mprotect
    // TODO(dannas): Call munmap.
    void *mem = mmap(nullptr, code.size(), PROT_WRITE | PROT_EXEC,
            MAP_ANON | MAP_PRIVATE, -1, 0);

    memcpy(mem, code.data(), code.size());

    int (*func)() = (int (*)())mem;

    int sum = func();
    ASSERT_EQ(8, sum);
    munmap(mem, code.size());
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
