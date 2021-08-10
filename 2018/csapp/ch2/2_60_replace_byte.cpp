#include <cassert>
#include <cstdint>
#include <iostream>
#include <iomanip>

#define ASSERT_EQ(expected, actual) \
    do { \
        if (expected != actual) \
            std::cout << __FILE__ <<":" << __LINE__ << ": " << std::hex << expected << " != " << actual << "\n"; \
	} while (0)


uint64_t replaceByte(uint64_t val, size_t pos, uint8_t c) {
    size_t shiftwidth = pos * 8;
    uint64_t mask = 0xff << shiftwidth;
    return (val & ~mask) | c << shiftwidth;

}

int main(int argc, char *argv[]) {

    uint64_t actual = replaceByte(0x00, 0, 0x01);
    ASSERT_EQ(0x01, actual);

	actual = replaceByte(0x000000, 0, 0xff);
    ASSERT_EQ(0x0000ff, actual);

	actual = replaceByte(0x000000, 1, 0xff);
    ASSERT_EQ(0x00ff00, actual);
}


