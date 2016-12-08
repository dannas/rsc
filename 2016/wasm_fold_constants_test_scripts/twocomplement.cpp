// NAME
//      twocomplement
//
// SYNOPSIS
//      twocomplement
// 
// DESCRIPTION
//      Prints the value representation in two complement-form for an integer
//      value. Reads from stdin expecting a type specifier and an integer
//      value.
//
//  BUGS
//      Don't support hexadecimal input
//      Don't support 8 bit types.

#include <iostream>
#include <climits>
#include <sstream>

#if __BYTEORDER__ != __LITTLE_ENDIAN__
#error Must be little endian
#endif

using namespace std;

char nibbleToHex(char c) {
    if (c < 10)
        return '0' + c;
    return 'a' + c - 10;
}

template <typename T>
string toHex(T x) {
    uint8_t *b = reinterpret_cast<uint8_t*>(&x);
    uint8_t *e = b + sizeof(x) - 1;
    string s;

    // Little endian, traverse in reverse order for value representation.
    // Endianess is a memory thing, we want the register representation.
    while (e >= b) {
        s += nibbleToHex((*e >> 4) & 0x0f);
        s += nibbleToHex(*e & 0x0f);
        e--;
    }
    return s;
}

// TODO(dannas): Turn this into a calculator? That displays the
// calculations with hexadecimal two-complement number representations.
int main(int argc, char* argv[]) {
    string type;
    string val;

    while (cin >> type) {
        if (type == "i64")  {
            int64_t x;
            cin >> x;
            cout << toHex(x) << "\n";
        } else if (type == "u64") {
            uint64_t x;
            cin >> x;
            cout << toHex(x) << "\n";
        } else if (type == "i32") {
            int32_t x;
            cin >> x;
            cout << toHex(x) << "\n";
        } else if (type == "u32") {
            uint32_t x;
            cin >> x;
            cout << toHex(x) << "\n";
        } else if (type == "i16") {
            int16_t x;
            cin >> x;
            cout << toHex(x) << "\n";
        } else if (type == "u16") {
            uint16_t x;
            cin >> x;
            cout << toHex(x) << "\n";
        // TODO(dannas): Add uint8_t and int8_t
        } else {
            cout << "ERROR: '" << type << "' is not known\n";
        }
    }
}
