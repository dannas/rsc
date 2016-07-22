#include <iostream>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <cassert>

// 49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d
// SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t

using namespace std;

int fromHex(int c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    throw new domain_error("hex val outside range");
}

vector<uint8_t> decodeHex(const string& hex) {
    vector<uint8_t> bytes;

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

int toBase64(int c) {

    // RFC 4048 section 4 Base 64 encoding
    static char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                             "abcdefghijklmnopqrstuvwxyz"
                             "0123456789+/";
    int N = sizeof(alphabet);

    assert(c >= 0 && c < N);

    return alphabet[c];
}

// RFC 4648
string encodeBase64(vector<uint8_t>& bytes) {
    string ret;
    size_t N = (bytes.size() + 2) * 4 / 3;
    ret.resize(N);

    int trailing = bytes.size()  % 3;

    // A 24-bit input group is formed by concatenating 3 8-bit input groups.
    // These 24 bits are then treated as 4 concatenated 6-bit groups, each of
    // which is translated into a single character in the base 64 alphabet.
    auto i = begin(bytes);
    auto out = begin(ret);
    for (; i != end(bytes) - trailing; i += 3) {
        uint32_t triple = (*i << 16) | (*(i+1) << 8) | *(i+2);

        *out++ = toBase64((triple >> 3 * 6) & 0x3f);
        *out++ = toBase64((triple >> 2 * 6) & 0x3f);
        *out++ = toBase64((triple >> 1 * 6) & 0x3f);
        *out++ = toBase64((triple >> 0 * 6) & 0x3f);
    }

    // Fill missing 6-bit quantas with '=' chars.
    if (trailing == 1) {
        uint32_t triple = *i;
        *out++ = toBase64((triple >> 1 * 6) & 0x3f);
        *out++ = toBase64((triple >> 0 * 6) & 0x3f);
        *out++ = '=';
        *out++ = '=';
    } else if (trailing ==  2) {
        uint32_t triple = (*(i+1) << 8) | *i;
        *out++ = toBase64((triple >> 2 * 6) & 0x3f);
        *out++ = toBase64((triple >> 1 * 6) & 0x3f);
        *out++ = toBase64((triple >> 0 * 6) & 0x3f);
        *out++ = '=';
    }
    return ret;
}

int main() {
    string hex = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";
    auto bytes = decodeHex(hex);
    auto base64 = encodeBase64(bytes);
    cout << base64 << "\n";

}
