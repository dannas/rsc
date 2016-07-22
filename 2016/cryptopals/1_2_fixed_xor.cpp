#include <vector>
#include <iostream>
#include <stdexcept>

// 1c0111001f010100061a024b53535009181c
// xor against
// 686974207468652062756c6c277320657965
// should produce
//746865206b696420646f6e277420706c6179

using namespace std;
using ByteVector = vector<uint8_t>;

int fromHex(int c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    throw new domain_error("hex val outside range");
}

int toHex(uint8_t c) {
    if (c >= 0 && c <= 9)
        return '0' + c;
    return 'a' + c - 10;
}

ByteVector decodeHex(const string& hex) {
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

string encodeHex(const ByteVector& bytes) {
    string s;
    for (auto c : bytes) {
        char hi = toHex(c >> 4);
        char lo = toHex(c & 0x0f);
        s.push_back(hi);
        s.push_back(lo);
    }
    return s;
}

ByteVector operator^(const ByteVector& x, const ByteVector& y) {
    if (x.size() != y.size())
        throw new invalid_argument("xor requires input of equal length");

    size_t N = x.size();
    ByteVector ret;
    ret.resize(N);
    for (size_t i = 0; i < N; ++i) {
        ret[i] = x[i] ^ y[i];
    }
    return ret;
}

int main() {
    auto x = decodeHex("1c0111001f010100061a024b53535009181c");
    auto y = decodeHex("686974207468652062756c6c277320657965");
    auto z = x ^ y;
    cout << encodeHex(z) << "\n";
}

