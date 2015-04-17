#include "String.h"

int main()
{
    String s;
    String t(s);
    String u = t;
    t = s;

    String w = "abc";
    t = "def";

}
