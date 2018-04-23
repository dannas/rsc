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

// Generate a matrix where rows and columns have been swapped. 
vector<ByteVector> transpose(const ByteVector& bytes, size_t ncols) {
	vector<ByteVector> ret;

	size_t nrows = bytes.size() + (ncols-1) / ncols;

	ret.resize(ncols, ByteVector(nrows));
#if 0
    find the elts in pos i*ncols
        find the elts in pos i*nrows

    1 2 3 
    4 5 6 
    7 8 9

    1 4 7
    2 5 8
    3 6 9

    (0, 0) => (0, 0)
    (
#endif

	for (size_t row = 0; row < nrows; ++row) {
		for (size_t col = 0; col < ncols; ++col) {
			ret[col][row] = bytes[row*ncols+ncols];
		}
	}
    return ret;
}
