#pragma once

#include <cstddef>
#include <cmath>

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
