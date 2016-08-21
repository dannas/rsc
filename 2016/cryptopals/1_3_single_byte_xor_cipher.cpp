#include <iostream>
#include <vector>
#include <stdexcept>
#include <cctype>

#include "util.h"

// 1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736
// has been xor'd against a single character
// find the key, decrypt the message
// You can do this by hand. But don't: write code to do it for you.

// How? Devise some method for "scoring" a piece of English plaintext. Character
// frequency is a good metric. Evaluate each output and choose the one with the
// best score. 

// NOTE: Order of frequency of characters in english text
// Etaoin shrdlu

using namespace std;
using ByteVector = vector<uint8_t>;
using HexString = string;

// Convert an ascii encoded character |c| to the corresponding hex value.
// Example:
//      9 => 9
//      a => 10
//      b => 11
int fromHex(uint8_t c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    throw new domain_error("hex val outside range");
}

// Decode the hex encoded string |hex| as a vector of byte values.
// Example:
//      "" => { }
//      "1" => { 1 }
//      "1122" => { 17, 34 }
//      "ffff" => { 255, 255 }
ByteVector decodeHex(const HexString& hex) {
    ByteVector bytes;
    if (hex.size() % 2 != 0)
        throw new invalid_argument("hex string must be multiple of two");

    for (auto i = begin(hex); i != end(hex);) {
        int hi = *i++;
        int lo = *i++;
        int c = (fromHex(hi) << 4) | fromHex(lo);
        bytes.push_back(c);
    }
    return bytes;
}

// Calculate ratio of printable characters in |bytes|.
double ratioPrintables(const ByteVector& bytes) {
    double n = 0;
    for (auto c : bytes) {
        if (isalpha(c)) {
            n++;
        }
    }
    return n / bytes.size();
}

// Xor |c| with each byte in |bytes|.
ByteVector operator^(const ByteVector& bytes, uint8_t c) {
    ByteVector ret;
    for (auto b : bytes) {
        ret.push_back(b ^ c);
    }
    return ret;
}

int main() {

    ByteVector best;
    double bestScore = 0;
    int bestKey = 0;
    auto bytes = decodeHex("1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736");
    for (int c = 0; c < 256; ++c) {
        auto b = bytes ^ c;
        double score = ratioPrintables(b);
        if (score > bestScore) {
            best = b;
            bestScore = score;
            bestKey = c;
        }
    }
    cout << (int)bestKey << "\t" << escapeString(best) << "\n";
}
