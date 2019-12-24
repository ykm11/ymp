#include "ymp.hpp"
#include "miscCalc.hpp"
#include <gmpxx.h>
#include <time.h>

/*
c++ -c ymp.cpp -o ymp.o -O3
c++ -c miscCalc.cpp -o miscCalc.o -O3
c++ -c benchmark.cpp -o benchmark.o -O3
c++ -o benchmark benchmark.o miscCalc.o ymp.o -lgmpxx -lgmp 
*/

void benchmark_ymp() {
    ymp_class a, b, c;
    a = ymp_class("e1AD904DBA973A66EF920C3D352ED1EEEBFD8A4E7F408AB1A97750E50B54B1266E588CBB6EC60B5B0E0AC2F53E8F772EE4E078663B17343E578236110E05130941CDE0D031B784BB1BEBA527BE923AA80AB2D04B055D8C702D1B9C8B9659B5F10057750C08755F7CA5454EEA7373B67D81FC1EB5514E1CAF288130A5237D4A47", 16);
    b = ymp_class("D36607539B3BFBB138F2C2B0E0F65C865079BC72BE1CB4079B889C5B88E8ED369699EE8BCD0D1220C0A6795F19D10156B175FC6427FB99F265EE1189F29A6D11C31A97141094CA0FC01BD3B3941E6843376229C9CF0FC6B7E89A6F366D94CA7B2EF93746A4EC3D533A4FD655B7721457C09097F72638CB7BA07E82EBAE4FF1CB", 16);
    c.setSize(a.N*2);
    const int N = 10000;
    size_t begin = clock();
    for(int i = 0; i < N; i++) {
        mul(c, a, b);
    }
    size_t end = clock();
    puts("[+] ymp mul");
    printf("\ttime = %fusec\n", (end - begin) / double(CLOCKS_PER_SEC) / N * 1e6);
}

void benchmark_gmp() {
    mpz_class a, b, c;
    a = mpz_class("e1AD904DBA973A66EF920C3D352ED1EEEBFD8A4E7F408AB1A97750E50B54B1266E588CBB6EC60B5B0E0AC2F53E8F772EE4E078663B17343E578236110E05130941CDE0D031B784BB1BEBA527BE923AA80AB2D04B055D8C702D1B9C8B9659B5F10057750C08755F7CA5454EEA7373B67D81FC1EB5514E1CAF288130A5237D4A47", 16);
    b = mpz_class("D36607539B3BFBB138F2C2B0E0F65C865079BC72BE1CB4079B889C5B88E8ED369699EE8BCD0D1220C0A6795F19D10156B175FC6427FB99F265EE1189F29A6D11C31A97141094CA0FC01BD3B3941E6843376229C9CF0FC6B7E89A6F366D94CA7B2EF93746A4EC3D533A4FD655B7721457C09097F72638CB7BA07E82EBAE4FF1CB", 16);
    const int N = 10000;
    size_t begin = clock();
    for(int i = 0; i < N; i++) {
        mpz_mul(c.get_mpz_t(), a.get_mpz_t(), b.get_mpz_t());
    }
    size_t end = clock();
    puts("[+] gmp mul");
    printf("\ttime = %fusec\n", (end - begin) / double(CLOCKS_PER_SEC) / N * 1e6);
}

void benchmark_sqr() {
    puts("[*] ymp mul(X, X) vs sqr(X)");
    ymp_class x, z;
    x = ymp_class("e1AD904DBA973A66EF920C3D352ED1EEEBFD8A4E7F408AB1A97750E50B54B1266E588CBB6EC60B5B0E0AC2F53E8F772EE4E078663B17343E578236110E05130941CDE0D031B784BB1BEBA527BE923AA80AB2D04B055D8C702D1B9C8B9659B5F10057750C08755F7CA5454EEA7373B67D81FC1EB5514E1CAF288130A5237D4A47", 16);
    z.setSize(x.N*2);
    const int N = 10000;
    size_t begin = clock();
    for(int i = 0; i < N; i++) {
        mul(z, x, x);
    }
    size_t end = clock();
    printf("\tmul(X, X)");
    printf("\ttime = %fusec\n", (end - begin) / double(CLOCKS_PER_SEC) / N * 1e6);

    memset(z.value, 0, z.N);
    begin = clock();
    for(int i = 0; i < N; i++) {
        sqr(z, x);
    }
    end = clock();
    printf("\tsqr(X)");
    printf("\t\ttime = %fusec\n", (end - begin) / double(CLOCKS_PER_SEC) / N * 1e6);
}

int main() {
    benchmark_gmp();
    benchmark_ymp();
    benchmark_sqr();
}
