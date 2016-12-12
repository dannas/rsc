// NAME
//      div - small program  for experimenting with div/mod
//
// DESCRIPTION
//      Demonstrates the differences between truncated division floored
//      division and euclidian division.
//
//      Truncated division is what is implemented in most silicon.
//
//      Floored division simplifies some modulo calculations. It's been
//      proposed by both Donald Knuth and Guy Steele.
//
// SEE ALSO
//      Leijen Daan. Division and Modulues for Computer Scientist. University
//      of Utrecht December 3, 2001.

#include <iostream>

using namespace std;

int truncatedDiv(int D, int d) {
    return D / d;
}

int truncatedMod(int D, int d) {
    return D % d;
}

int flooredDiv(int D, int d) {
    int q = D / d;
    int r = D % d;
    if ((r > 0 && d < 0) || (r < 0 && d > 0))
        q--;
    return q;
}

int flooredMod(int D, int d) {
    int r = D % d;
    if ((r > 0 && d < 0) || (r < 0 && d > 0))
        r += d;
    return r;
}

int euclidianDiv(int D, int d) {
    int q = D / d;
    int r = D % d;
    if (r < 0) {
        if (d > 0) 
            q--;
        else
            q++;
    }
    return q;
}

int euclidianMod(int D, int d) {
    int r = D % d;

    if (r < 0) {
        if (d > 0)
            r++;
        else
            r--;
    }
    return r;
}

int main() {
    char c;
    int x, y;

    while (cin >> c >> x >> y) {

        int (*div)(int, int);
        int (*mod)(int, int);

        if (c == 't') {
            div = truncatedDiv;
            mod = truncatedMod;
        } else if (c == 'f') {
            div = flooredDiv;
            mod = flooredMod;
        } else if (c == 'e') {
            div = euclidianDiv;
            mod = euclidianMod;
        } else {
            break;
        }

        cout << "\t" << div(x, y) << " " << mod(x, y) << "\n";
    }
}
