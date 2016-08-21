#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "util.h"

// One of the 60-character strings in this file has been encrypted by single-character XOR.
// 4.txt

// Find it. 

using namespace std;
using ByteVector = vector<uint8_t>;
using HexString = string;

// Convert an ascii encoded character |c| representing a hex value into a nibble.
// Example:
//      '9' => 9
//      'a' => 10
//      'b' => 11
int fromHex(uint8_t c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    throw new domain_error("hex val outside range");
}

// Convert a nibble |c| into the corresponding hex ascii char.
// Example:
//      0 => '0'
//      10 => 'a'
//      15 => 'f'
int toHex(uint8_t c) {
    if (c >= 0 && c <= 9)
        return '0' + c;
    return 'a' + c - 10;
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
    ByteVector ret(bytes.size());
    auto i = begin(ret);
    for (auto b : bytes) {
        *i++ = b ^ c;
    }
    return ret;
}

struct PlainText {
    ByteVector bytes;   // after applying |key| to original text
    double score;
    uint8_t key;
};

int main() {
    ifstream f;
    f.open("4.txt");
    string s;
    vector<PlainText> p;

    // Store all combinations of N input lines and M keys.
    while (getline(f, s)) {
        auto bytes = decodeHex(s);
        for (uint8_t key = 0; ; ++key) {
            auto decoded = bytes ^ key;
            p.push_back({decoded, ratioPrintables(decoded), key});
            if (key == 255)
                break;
        }
    }

    // Sort on ratio printables in reversed order.
    sort(begin(p), end(p), [](auto& x, auto& y) { return x.score > y.score; });

    // Print first page or so, looking for an obvious match.
    for (size_t i = 0; i < 25; ++i) {
        int key = p[i].key;
        string t = escapeString(p[i].bytes);
        cout << key << " " << t << "\n";
    }
}
