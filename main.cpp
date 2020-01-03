#include "ymp.hpp"

#include <iostream>

int main() {
    ymp_class a, b, c;
    b = ymp_class("10000000", 16);
    a = ymp_class("1", 16);
    dump(c);
    std::cout << "a = ";
    dump(a);
    std::cout << "b = ";
    dump(b);

    std::cout << "Add:\n";
    add(c, a, b);
    dump(c);

    std::cout << "Sub:\n";
    sub(c, a, b);
    dump(c);

    std::cout << "Mul:\n";
    mul(a, a, b);
    dump(a);
    sqr(c, c);
    dump(c);

    ymp_class x, y;
    x = -110;
    y = -110;
    printf("%d\n", cmp(x, y));

}
