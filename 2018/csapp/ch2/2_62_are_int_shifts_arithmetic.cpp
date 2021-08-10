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

bool isIntShiftArithmetic() {
	// -1 has binary representation 0xFF...FF
	// Arithmetic shifts should preserve sign
	return (-1 >> 1) == -1;
}

int main()
{
	ASSERT_TRUE(isIntShiftArithmetic());
}
