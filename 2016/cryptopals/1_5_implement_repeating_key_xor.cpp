#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

// Implement repeating-key XOR
//
// Here is the opening stanza of an important work of the English language:
//
// Burning 'em, if you ain't quick and nimble
// I go crazy when I hear a cymbal
//
// Encrypt it, under the key "ICE", using repeating-key XOR.
//
// In repeating-key XOR, you'll sequentially apply each byte of the key; the
// first byte of plaintext will be XOR'd against I, the next C, the next E,
// then I again for the 4th byte, and so on.
//
// It should come out to:
//
//  0b3637272a2b2e63622c2e69692a23693a2a3c6324202d623d63343c2a26226324272765272
//  a282b2f20430a652e2c652a3124333a653e2b2027630c692b20283165286326302e27282f
//
// Encrypt a bunch of stuff using your repeating-key XOR function. Encrypt your
// mail. Encrypt your password file. Your .sig file. Get a feel for it. I
// promise, we aren't wasting your time with this.

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

ByteVector encrypt(const ByteVector& plaintext, const ByteVector& key) {

    size_t N = plaintext.size();
    size_t M = key.size();
    ByteVector res(N);
    for (size_t i = 0; i < N; ++i) {
        size_t j = i % M;
        res[i] = plaintext[i] ^ key[j];
    }
    return res;
}

HexString encodeHex(const ByteVector& bytes) {
    size_t N = bytes.size();
    HexString s;
    s.resize(N * 2);
    auto i = begin(s);
    for (auto b : bytes) {
        uint8_t hi = b >> 4;
        uint8_t lo = b & 0x0f;
        *i++ = toHex(hi);
        *i++ = toHex(lo);

    }
    return s;
}

int main() {

    string s = "Burning 'em, if you ain't quick and nimble\n"
               "I go crazy when I hear a cymbal";

    ByteVector bytes(begin(s), end(s));

    ByteVector key = {'I', 'C', 'E'};
    auto encrypted = encrypt(bytes, key);

    cout << encodeHex(encrypted) << "\n";
}
