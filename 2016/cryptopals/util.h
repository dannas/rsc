#pragma once

#include <cstddef>
#include <cmath>
#include <string>
#include <vector>
#include <cstdint>

using ByteVector = std::vector<uint8_t>;
using HexString = std::string;

// Calculates a one-way chi-square test, comparing the frequencies of the |N|
// |observed| values to the frequencies of the |N| |expected| values.
template <size_t N>
double chisquare(int (&observed)[N], int (&expected)[N]) {
    double ret = 0;

    // TODO(dannas): I'm ignoring columns where the expected frequency is zero.
    // Does that make sense?
    for (size_t i = 0; i < N; ++i) {
        if (expected[i] > 0)
            ret += pow(observed[i] - expected[i], 2) / expected[i];
    }
    return ret;
}

// Encode |bytes| as a string of ascii characters, where non-printables are
// represented on the form \xYY.
// Example:
//      { 0x00, 0x61, 0x62, 0x63, 0x19 } => "\x00abc\x19"
std::string escapeString(const ByteVector& bytes);
