#include "ymp.hpp"

#include <iostream>

int main() {
    ymp_class a, b, c;
    a = ymp_class("100FFDEADBEEF321382183029138201103823284291402840211A1", 16);
    b = ymp_class("100330F28310381983842830B1ADEADBEEF2132913890218321008", 16);
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

}
