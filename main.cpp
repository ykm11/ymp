#include "ymp.hpp"

int main() {
    ymp_class a, b, c;
    a = ymp_class("12345123201832849018390218382031", 16);
    b = ymp_class("FFFF321302183281048183802814931483080FFF", 16);
    dump(c);
    dump(a);
    dump(b);

    add(c, a, b);
    dump(c);

    mul(c, a, a);
    dump(c);

    memset(c.value, 0, c.N);
    sqr(c, a);
    dump(c);

}
