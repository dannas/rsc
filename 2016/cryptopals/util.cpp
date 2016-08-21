#include "util.h"

#include <cctype>

using namespace std;

string escapeString(const ByteVector& bytes) {
    string ret;
    for (auto c : bytes) {
        if (isprint(c)) {
            ret.push_back(c);
        } else {
            ret.push_back('\\');
            ret.push_back('x');
            char hi = c >> 4 & 0x0f;
            char lo = c & 0x0f;
            ret.push_back(hi + '0');
            ret.push_back(lo + '0');
        }
    }
    return ret;
}
