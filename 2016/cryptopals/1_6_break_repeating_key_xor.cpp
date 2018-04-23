#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <iterator>
#include <cstring>
#include <cmath>
#include <cfloat>
#include <cassert>

#include "util.h"

// This challenge isn't conceptually hard, but it involves actual error-prone
// coding. The other challenges in this set are there to bring you up to speed.
// This one is there to qualify you. If you can do this one, you're probably
// just fine up to Set 6.

// There's a file here. It's been base64'd after being encrypted with
// repeating-key XOR.
// 
// Decrypt it.
// 
// Here's how:
// 
//     Let KEYSIZE be the guessed length of the key; try values from 2 to (say)
//     40.  Write a function to compute the edit distance/Hamming distance
//     between two strings. The Hamming distance is just the number of
//     differing bits. The distance between:
// 
//     this is a test
// 
//     and
// 
//     wokka wokka!!!
// 
//     is 37. Make sure your code agrees before you proceed.
//
//     For each KEYSIZE, take the first KEYSIZE worth of bytes, and the second
//     KEYSIZE worth of bytes, and find the edit distance between them.
//     Normalize this result by dividing by KEYSIZE.  The KEYSIZE with the
//     smallest normalized edit distance is probably the key. You could proceed
//     perhaps with the smallest 2-3 KEYSIZE values. Or take 4 KEYSIZE blocks
//     instead of 2 and average the distances.  Now that you probably know the
//     KEYSIZE: break the ciphertext into blocks of KEYSIZE length.  Now
//     transpose the blocks: make a block that is the first byte of every
//     block, and a block that is the second byte of every block, and so on.
//     Solve each block as if it was single-character XOR. You already have
//     code to do this.  For each block, the single-byte XOR key that produces
//     the best looking histogram is the repeating-key XOR key byte for that
//     block. Put them together and you have the key.
// 
// This code is going to turn out to be surprisingly useful later on. Breaking
// repeating-key XOR ("Vigenere") statistically is obviously an academic
// exercise, a "Crypto 101" thing. But more people "know how" to break it than
// can actually break it, and a similar technique breaks something much more
// important.
// No, that's not a mistake.
// 
// We get more tech support questions for this challenge than any of the other
// ones. We promise, there aren't any blatant errors in this text. In
// particular: the "wokka wokka!!!" edit distance really is 37.

using namespace std;
using ByteVector = vector<uint8_t>;
using HexString = string;

// Return number of non-zero bits in |x|.
// ### Naive implementation. Replace with gcc builtin or Kernighan algorithm.
int popcount(uint8_t x) {
	int n = 0;
	for (; x; x >>= 1) {
		if (x & 1)
			n++;
	}
	return n;
}

int distance(const ByteVector& x, const ByteVector& y) {
	if (x.size() != y.size()) {
		throw new invalid_argument("hamming distance: vectors length differs");
	}

	int n = 0;

	for (size_t i = 0; i < x.size(); ++i) {
		uint8_t diff = x[i] ^ y[i];
		n += popcount(diff);
	}
	return n;
}

double normalizedDistance(const ByteVector& x, const ByteVector& y) {
	double N = x.size();
	double dist = distance(x, y);
	return dist / N;
}

ByteVector readBytes(const char* fname) {
	ifstream f(fname);
	istream_iterator<uint8_t> start(f), end;
	ByteVector bytes(start, end);
	return bytes;
}

struct KeysizeRating {
	size_t size;
	double rating;
};

bool operator<(const KeysizeRating& lhs, const KeysizeRating& rhs) {
	return rhs.rating < lhs.rating;
}

// Xor |c| with each byte in |bytes|.
ByteVector operator^(const ByteVector& bytes, uint8_t c) {
    ByteVector ret;
    for (auto b : bytes) {
        ret.push_back(b ^ c);
    }
    return ret;
}

// Distribution of letters for typical english text.
// Generated from the Leipzig Corpora Collection.
// Downloaded from:
// http://practicalcryptography.com/cryptanalysis/letter-frequencies-various-languages/english-letter-frequencies/
const double kLetterDistribution[] = {
/* A = */   0.0855,        
/* B = */   0.0160,        
/* C = */   0.0316,        
/* D = */   0.0387,        
/* E = */   0.0210,        
/* F = */   0.0218,        
/* G = */   0.0209,        
/* H = */   0.0496,        
/* I = */   0.0733,        
/* J = */   0.0022,        
/* K = */   0.0081,
/* L = */   0.0421,
/* M = */   0.0253,
/* N = */   0.0717,
/* O = */   0.0747,
/* P = */   0.0207,
/* Q = */   0.0010,
/* R = */   0.0633,
/* S = */   0.0673,
/* T = */   0.0894,
/* U = */   0.0268,
/* V = */   0.0106,
/* W = */   0.0183,
/* X = */   0.0019,
/* Y = */   0.0172,
/* Z = */   0.0011,
};

const size_t kSizeAlphabet = sizeof(kLetterDistribution)/sizeof(kLetterDistribution[0]);

template <size_t N>
int* computeHistogram(const ByteVector& decoded, int (&hist)[N]){
    memset(hist, 0, N);
    for (uint8_t c : decoded) {
        c = tolower(c);
        if (c >= 'a' && c <= 'z') {
            size_t pos = c - 'a';
            assert (pos >= 0 && pos < N);

            hist[pos]++;
        }
    }
    return hist;
}

template <size_t N>
int* scaleHistogram(int (&hist)[N], int scale) {
    for (size_t i = 0; i < N; ++i) {
        hist[i] = ceil(kLetterDistribution[i] * scale);
    }
    return hist;
}

uint8_t findClosestMatch(const ByteVector& bytes) {

    int hist[26];
    int referenceHist[26];
    struct Fit { double diff; uint8_t key; } best {DBL_MAX, 0};
    scaleHistogram(referenceHist, bytes.size());

    for (uint8_t key = 0; ; ++key) {
        auto decoded = bytes ^ key;
        computeHistogram(decoded, hist);
        double diff = chisquare(referenceHist, hist);
        //cout << "\t" << diff << "\n";
        if (diff < best.diff) {
            best = {diff, key};
        }
        if (key == 255)
            break;
    }
    return best.key;
}

int main() {
	string s = "this is a test";
	string t = "wokka wokka!!!";

	ByteVector x(begin(s), end(s));
	ByteVector y(begin(t), end(t));
	cout << distance(x, y) << "\n";

	ByteVector bytes = readBytes("6.txt");
	vector<KeysizeRating> ratings;

    auto i = begin(bytes);
    for (size_t sz = 2; sz <= 40; ++sz) {
        ByteVector a(i, i+sz);
        ByteVector b(i+sz, i+sz+sz);
        double r = normalizedDistance(a, b);
        ratings.push_back({sz, r});
    }

	sort(begin(ratings), end(ratings));
	for (auto r : ratings) {
		cout << r.rating << " " << r.size << "\n";
	}

    // The experiment above showed that N=20 was the best fit for key size.
	vector<ByteVector> transposed;
	size_t N = bytes.size() + 19 / 20;
	transposed.resize(29, ByteVector(N));

	for (size_t i = 0; i < bytes.size(); i += 7) {
		for (size_t j = 0; j < 20; ++j) {
			transposed[j][i/20] = bytes[i+j];
		}
	}

    ByteVector key(20);

    for (size_t i = 0; i < key.size(); ++i) {
        key[i] = findClosestMatch(transposed[i]);
    }

    cout << escapeString(key) << "\n";

    // TODO(dannas): Use a chi-square test to identify closest fit for our
    // categorical distribution of letters.
    // http://practicalcryptography.com/cryptanalysis/text-characterisation/chi-squared-statistic/
    // https://www.quora.com/What-is-the-most-intuitive-explanation-for-the-chi-square-test/answer/Matthew-Handy?srid=oX1S
}
