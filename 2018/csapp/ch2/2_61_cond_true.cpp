#include <cstdint>
#include <iostream>
#include <iomanip>

#define ASSERT_EQ(expected, actual) \
    do { \
        if (expected != actual) \
            std::cout << __FILE__ <<":" << __LINE__ << ": " << std::hex << expected << " != " << actual << "\n"; \
	} while (0)

#define ASSERT_TRUE(expr) \
    do { \
        if (!(expr)) \
            std::cout << __FILE__ <<":" << __LINE__ << ": '" << #expr << "' is false\n"; \
	} while (0)

#define ASSERT_FALSE(expr) \
    do { \
        if ((expr)) \
            std::cout << __FILE__ <<":" << __LINE__ << ": '" << #expr << "' is true\n"; \
	} while (0)


bool anyBitIsOne(uint32_t val) {
	return val;
}

bool anyBitIsZero(uint32_t val) {
	return ~val;
}

bool anyBitInLsbIsOne(uint32_t val) {
	return val & 0xff;
}

bool anyBitInLsbIsZero(uint32_t val) {
	return ~val & 0xff;
}

int main(int argc, char *argv[])
{
	ASSERT_TRUE(anyBitIsOne(0x01));
	ASSERT_FALSE(anyBitIsOne(0x00));

	ASSERT_TRUE(anyBitInLsbIsOne(0x1001));
}
