#pragma once
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <string>

class ymp_class;
void dump(const ymp_class &v);
void dump(const unsigned char v[], size_t n);
void add(ymp_class &z, const ymp_class &x, const ymp_class &y);
void sub(ymp_class &z, const ymp_class &x, const ymp_class &y);
void mul(ymp_class &z, const ymp_class &x, const ymp_class &y);
void sqr(ymp_class &z, const ymp_class &x);
void div(ymp_class &z, const ymp_class &x, const ymp_class &y);
char cmp(const ymp_class &x, const ymp_class &y);

inline void valCopy(ymp_class &z, const ymp_class &x);
inline void lshift(unsigned char v[], size_t n, size_t shift);
inline bool isStrNops(size_t found);


class ymp_class {
public:
    size_t N;
    unsigned char *value;
    bool isNeg;

    ymp_class() : N(1), value(NULL), isNeg(false) {}
    ymp_class(int val) : N(1), isNeg(false) {
        unsigned char *ptr;
        if (val == 0) {
            ptr = (unsigned char*)malloc(1*sizeof(unsigned char));
            if (ptr == NULL) {
                abort();
            }
            value = ptr;
            value[0] = 0;
            return;
        }

        if (val < 0) {
            isNeg = true;
            val = -val;
        }
        while (((val >> (N*8)) & 0xFF) != 0x00) {
            N++;
        }
        ptr = (unsigned char*)malloc(N*sizeof(unsigned char));
        if (ptr == NULL) {
            abort();
        }
        value = ptr;
        for (size_t i = 0; i < N; i++) {
            value[i] = (val >> (8*i)) & 0xFF;
        }        
    }

    ymp_class(std::string str, unsigned char base) {
        switch (base) {
            case 16: {
                std::transform(str.begin(), str.end(), str.begin(), ::toupper);
                const std::string hex = "0123456789ABCDEF";
                size_t l = str.length();
                N = (l + 1)/2;
                unsigned char *ptr;
                ptr = (unsigned char*)malloc(N*sizeof(unsigned char));
                if (ptr == NULL) {
                    abort();
                }
                value = ptr;                
                size_t found; // Invalid のときはエラーを投げる
                if ((l & 1) == 0) {
                    for (size_t i = 0; i < N; i++) {
                        found = hex.find(str[2*i]);
                        if (isStrNops(found)) {
                            fputs("Invalid sqeuance", stderr);
                            abort();
                        }
                        value[N-i-1] = ((unsigned char)found) << 4;

                        found = hex.find(str[2*i+1]);
                        if (isStrNops(found)) {
                            fputs("Invalid sqeuance", stderr);
                            abort();
                        }
                        value[N-i-1] += (unsigned char)found;
                    }
                } else {
                    found = hex.find(str[0]);
                    if (isStrNops(found)) {
                            fputs("Invalid sqeuance", stderr);
                            abort();
                    }   
                    value[N-1] = (unsigned char)found;
                    for (size_t i = 1; i < N; i++) {
                        found = hex.find(str[2*i-1]);
                        if (isStrNops(found)) {
                            fputs("Invalid sqeuance", stderr);
                            abort();
                        }                    
                        value[N-i-1] = ((unsigned char)found) << 4;

                        found = hex.find(str[2*i]);
                        if (isStrNops(found)) {
                            fputs("Invalid sqeuance", stderr);
                            abort();
                        }                    
                        value[N-i-1] += ((unsigned char)found);
                    }
                }
                isNeg = false;
                break;
            }
            default:
                fputs("Not Implemented\n", stderr);
                abort();
        }
    }

    ymp_class operator+(const ymp_class &x) const {
        ymp_class z;
        add(z, *this, x);
        return z;
    }

    ymp_class operator-(const ymp_class &x) const {
        ymp_class z;
        sub(z, *this, x);
        return z;
    }

    ymp_class operator*(const ymp_class &x) const {
        ymp_class z;
        mul(z, *this, x);
        return z;
    }

    void setSize(size_t new_size);
    static void neg(ymp_class &z);
};


inline void valCopy(ymp_class &z, const ymp_class &x) {
    z.setSize(x.N);
    z.isNeg = x.isNeg;
    for (size_t i = 0; i < x.N; i++) z.value[i] = x.value[i];
}

inline void removeHeadZero(ymp_class &z, const ymp_class &x) {
    size_t k = x.N;
    unsigned char *tmp;
    while (x.value[k-1] == 0x00) {
        k--;
    }
    if (k == x.N) {
        return;
    }

    tmp = (unsigned char*)malloc(k*sizeof(unsigned char));
    if (tmp == NULL) {
        abort();
    }
    for (size_t i = 0; i < k; i++) tmp[i] = x.value[i];

    z.setSize(k);
    for (size_t i = 0; i < k; i++) z.value[i] = tmp[i];
    free(tmp); 
}


inline void lshift(unsigned char v[], size_t n, size_t shift) {
    v[n-1] <<= shift;
    for (size_t i = n-1; i > 0; i--) {
        v[i] += (v[i-1] >> (8 - shift));
        v[i-1] <<= shift;
    }
}

inline bool isStrNops(size_t found) {
    return (found == std::string::npos);
}

