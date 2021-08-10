#include <cstdint>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <climits>

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

bool anyOddBit(uint32_t val) {
    // Mask out 1010 from each nibble.
    return val & 0xaaaaaaaa;
}

int popCount(uint32_t val) {
    // Divide and conquer.
    // Sum 2 bits, then 4,then 8, then 16.
    val = (val & 0x55555555) + ((val >> 1) & 0x55555555);
    val = (val & 0x33333333) + ((val >> 2) & 0x33333333);
    val = (val & 0x0f0f0f0f) + ((val >> 4) & 0x0f0f0f0f);
    val = (val & 0x00ff00ff) + ((val >> 8) & 0x00ff00ff);
    val = (val & 0x0000ffff) + ((val >> 16) & 0x0000ffff);
    return val;
}

bool oddOnes(uint32_t val) {
    // Preserve parity of ones.
    val ^= val >> 16;
    val ^= val >> 8;
    val ^= val >> 4;
    val ^= val >> 2;
    val ^= val >> 1;
    return val & 0x1;
}

uint32_t leftmost_one(uint32_t val) {
    // Make sure all bits to the right are set to 1.
    val |= val >> 16;
    val |= val >> 8;
    val |= val >> 4;
    val |= val >> 2;
    val |= val >> 1;
    // All bits to the right of msb is 1.
    return val ^ (val >> 1);
}

// 2.68
int lower_one_mask(int n) {
    return ~0u >> ((sizeof(n) << 3) - n);
}

// 2.69
unsigned rotate_left(unsigned x, int n) {
    // Number of bits.
    int w = sizeof(x) << 3;
    // Mask out the upper part. |n| is the number of leftmost bits that should
    // be rotated.
    unsigned upper = x & (~0u << (w-n));
    // Shift the lower part |n| steps.
    // Shift the upper part down.
    return (x << n) | (upper >> (w-n));
}

// 2.70
int fits_bits(int x, int n) {
    // Mask out all bits above n and msb.
    int mask = (~0u >> (n-1)) << (n-1);
    // A positive number is within range if msb=0 and higherBits are 0
    // A negative number is within range if msb=1 and all higher bits are 1.
    return x == 0 || x == mask;
}

// 2.71
int xbyte(unsigned word, int bytenum) {
    // We do sign extension by arithmetic shift.
    return ((int)word << ((3-bytenum) << 3)) >> 24;
}

// 2.72
void copy_int(int val, void *buf, int maxbytes) {
    if ((unsigned)maxbytes >= sizeof(val))
        memcpy(buf, (void*) &val, sizeof(val));
}

// 2.73
int saturating_add(int x, int y) {
    int w = sizeof(x) << 3;
    int r = x + y;

    // Mask out msb for operands and result.
    int msb_x = x & (1 << (w-1));
    int msb_y = y & (1 << (w-1));
    int msb_r = r & (1 << (w-1));

    // Overflow happens when x and y is positive and result is negative.
    int overflow = !msb_x && !msb_y && msb_r;
    // Underflow happens when x and y is negative and result is positive.
    int underflow = msb_x && msb_y && !msb_r;

    // Clear msb for overflow.
    r &= ~0u >> overflow;
    // Set all lower bits for overflow.
    r |= (unsigned)-overflow >> 1;

    // Clear lower w-1 bits for underflow.
    r &= (unsigned)(-1+underflow);
    // Set msb for underflow.
    r |= underflow << (w-1);

    return r;
}

// 2.74
int tsub_ok(int x, int y) {
    // x + y underflows if x and y is negative
    // x - y underflows if x is negative and y is positive
    // I can't just inspect msb.
    // I can't use relative comparisons.
    // How about using masks for checking overlaps?
    //
    // How does subtraction work in an ALU? I've forgotten?
    // 3 - 7 = 10 + 3 - 10 - 7 = (10 + 3) 
    //
    // Some examples with nibbles:
    //                            twos comp of y
    //      1111 - 1111 => ok       0001
    //      1111 - 0001 => ok       1111
    //      1000 - 0001 => fail     1111
    //      1001 - 0010 => fail     1100
    //      1010 - 0011 => fail     1101
    //      1011 - 0110 => fail     1000
    //
    //  Looks like there's an underflow if y > x when msb(x) is masked out.
    int w = sizeof(x) << 3;
    int r = x - y;
    int mask = 1 << (w-1);
    int x_msb = x & mask;
    int y_msb = y & mask;
    int r_msb = r & mask;
    // A subtraction underflow occurs if y < 0, x > 0 and r > 0
    int underflow = x_msb && !y_msb && !r_msb;
    return !underflow;
}

// 2.75


int main(int argc, char *argv[])
{
    uint32_t val = 0x00000000;
    int expected = 0;
    int actual = popCount(val);
    ASSERT_EQ(expected, actual);

    val = 0xff;
    expected = 8;
    actual = popCount(val);
    ASSERT_EQ(expected, actual);

    val = 0x0;
    expected = false;
    actual = oddOnes(val);
    ASSERT_EQ(expected, actual);

    val = 0x1;
    expected = true;
    actual = oddOnes(val);
    ASSERT_EQ(expected, actual);

    val = 0x3;
    expected= false;
    actual = oddOnes(val);
    ASSERT_EQ(expected, actual);

    val = 0x12345678;
    expected = 0x23456781;
    actual = rotate_left(val, 4);
    ASSERT_EQ(expected, actual);

    unsigned word = 0xff;
    expected = -1;
    actual = xbyte(word, 0);
    ASSERT_EQ(expected, actual);

    expected = 42;
    actual = saturating_add(14, 28);
    ASSERT_EQ(expected, actual);

    expected = INT_MIN;
    actual = saturating_add(INT_MIN, -1);
    ASSERT_EQ(expected, actual);
}
