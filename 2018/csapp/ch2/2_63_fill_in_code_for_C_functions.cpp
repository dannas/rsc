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

uint32_t srl(uint32_t val, int k) {
	// Arithmetic shift.
	uint32_t sra = static_cast<int32_t>(val) >> k;

	// Mask for upper k bits.
	// The expression 0 & val ensures 0 has the correct size.
	uint32_t mask = ~(0 & val) << (31 - k);

	return sra & ~mask;
}

int32_t sra(int32_t val, int k) {
	// Logical shift.
	int32_t srl = static_cast<uint32_t>(val) >> k;

    // Extract msb
	uint32_t highBit = static_cast<uint32_t>(val) >> 31;

    // Set to all ones if highBit=1.
    int32_t leading = 0 - highBit;

    // Mask for upper k bits.
    leading = leading << (31-k);

    return leading | srl;
}

int main(int argc, char *argv[])
{
    {
        uint32_t val = ~static_cast<uint32_t>(0);
        uint32_t expected = val >> 5;
        uint32_t actual = srl(val, 5);
        ASSERT_EQ(expected, actual);

        expected = val >> 1;
        actual = srl(val, 1);
        ASSERT_EQ(expected, actual);

        expected = val >> 9;
        actual = srl(val, 9);
        ASSERT_EQ(expected, actual);
    }
    {
        int32_t val = -1;
        int32_t expected = val >> 5;
        int32_t actual = sra(val, 5);
        ASSERT_EQ(expected, actual);

        val = -17;
        expected = val >> 31;
        actual = sra(val, 31);
        ASSERT_EQ(expected, actual);
    }
}
