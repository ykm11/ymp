#include "ymp.hpp"
#include "miscCalc.hpp"
#include <limits.h>
#include <stdio.h>
#include <cstring>

void ymp_class::setSize(size_t new_size) {
    if (value != NULL) {
        memset(value, 0, N);
        free(value);
    }
    unsigned char *ptr = (unsigned char*)malloc(new_size*sizeof(unsigned char));
    if (ptr == NULL) {
        fprintf(stderr, "Memory allocate error");
        abort();
    }
    value = ptr;
    N = new_size;
    memset(value, 0, N);
}

void dump(const unsigned char v[], size_t n) {
    printf("0x");
    for (size_t i = 0; i < n; i++) {
        printf("%02X", v[n-i-1]);
    }
    puts("");
}

void dump(const ymp_class &v) {
    if (v.value == NULL) {
        puts("Null");
        return;
    }
    printf("0x");
    for (size_t i = 0; i < v.N; i++) {
        printf("%02X", v.value[v.N-i-1]);
    }
    puts("");
}

int cmp(const ymp_class &x, const ymp_class &y) {
    if (x.N > y.N) {
        return 1;
    } else if (x.N < y.N) {
        return -1;
    }

    for (size_t i = x.N; i > 0; i--) {
        if (x.value[i-1] > y.value[i-1]) {
            return 1;
        } else if(x.value[i-1] < y.value[i-1]) {
            return -1;
        }
    }
    return 0;
}

void add(ymp_class &z, const ymp_class &x, const ymp_class &y) {
    unsigned char carry = 0;
#if 1
    if (x.N < y.N) {
        valCopy(z, y);
        for (size_t i = 0; i < x.N; i++) {
            if ((UCHAR_MAX - z.value[i]) < x.value[i]) {
                z.value[i] = z.value[i] + x.value[i] + carry;
                carry = 1;
            } else {
                if (z.value[i] + x.value[i] == UCHAR_MAX) {
                    if (carry == 0) {
                        z.value[i] = UCHAR_MAX;
                    } else {
                        z.value[i] = 0;
                        carry = 1;
                    }
                } else {
                    z.value[i] = z.value[i] + x.value[i] + carry;
                    carry = 0;
                }
            }
        }
        if (carry == 1) {
            for (size_t k = 0; x.N+k < z.N; k++) {
                if (z.value[x.N+k] == UCHAR_MAX) {
                    z.value[x.N+k] = 0;
                } else {
                    z.value[x.N+k] += 1;
                    break;
                }
            }
        }
        return;
    } else if (x.N > y.N) {
        valCopy(z, x);
        for (size_t i = 0; i < y.N; i++) {
            if ((UCHAR_MAX - z.value[i]) < y.value[i]) {
                z.value[i] = z.value[i] + y.value[i] + carry;
                carry = 1;
            } else {
                if (z.value[i] + y.value[i] == UCHAR_MAX) {
                    if (carry == 0) {
                        z.value[i] = UCHAR_MAX;
                    } else {
                        z.value[i] = 0;
                        carry = 1;
                    }
                } else {
                    z.value[i] = z.value[i] + y.value[i] + carry;
                    carry = 0;
                }
            }
        }
        if (carry == 1) {
            for (size_t k = 0; y.N+k < z.N; k++) {
                if (z.value[y.N+k] == UCHAR_MAX) {
                    z.value[y.N+k] = 0;
                } else {
                    z.value[y.N+k] += 1;
                    break;
                }
            }
        }
        return;
    }
    valCopy(z, x);
    for (size_t i = 0; i < y.N; i++) {
        if ((UCHAR_MAX - z.value[i]) < y.value[i]) {
            z.value[i] = z.value[i] + y.value[i] + carry;
            carry = 1;
        } else {
            if (z.value[i] + y.value[i] == UCHAR_MAX) {
                if (carry == 0) {
                    z.value[i] = UCHAR_MAX;
                } else {
                    z.value[i] = 0;
                    carry = 1;
                }
            } else {
                z.value[i] = z.value[i] + y.value[i] + carry;
                carry = 0;
            }
        }
    }
    return;
#else
    for (size_t i = 0; i < x.N; i++) {
        if ((UCHAR_MAX - x.value[i]) < y.value[i]) {
            z.value[i] = x.value[i] + y.value[i] + carry;
            carry = 1;
        } else {
            if (x.value[i] + y.value[i] == UCHAR_MAX) {
                if (carry == 0) {
                    z.value[i] = UCHAR_MAX;
                } else {
                    z.value[i] = 0;
                    carry = 1;
                }
            } else {
                z.value[i] = x.value[i] + y.value[i] + carry;
                carry = 0;
            }
        }
    }
#endif
}

void mul(ymp_class &z, const ymp_class &x, const ymp_class &y) {
    z.setSize(2*x.N);
    unsigned char t[2]; 
    unsigned char carry;
    for (size_t i = 0; i < x.N; i++) {
        for (size_t j = 0; j < y.N; j++) {
            carry = 0;
            mul(t, x.value[i], y.value[j]);
            if (UCHAR_MAX - z.value[i+j] < t[0]) {
                carry = 1;
            }
            z.value[i+j] += t[0];
              
            if (UCHAR_MAX - z.value[i+j+1] < t[1]) { // carry 発生
                z.value[i+j+1] += t[1] + carry;
                for (size_t k = 2; i+j+k < z.N-1; k++) {
                    if (z.value[i+j+k] == UCHAR_MAX) {
                        z.value[i+j+k] = 0;
                    } else {
                        z.value[i+j+k] += 1;
                        break;
                    }
                }
            } else {
                if (carry == 1 && z.value[i+j+1] + t[1] == UCHAR_MAX) {
                    z.value[i+j+1] = 0;
                    for (size_t k = 2; i+j+k < z.N-1; k++) {
                        if (z.value[i+j+k] == UCHAR_MAX) {
                            z.value[i+j+k] = 0;
                        } else {
                            z.value[i+j+k] += 1;
                            break;
                        }
                    }
                } else {
                    z.value[i+j+1] += t[1] + carry;
                }
            }
        }
    }
}

void sqr(ymp_class &z, const ymp_class &x) {
    z.setSize(2*x.N);
    unsigned char t[2]; 
    unsigned char carry;
    for (size_t i = 0; i < x.N; i++) {
        for (size_t j = i+1; j < x.N; j++) {
            carry = 0;
            mul(t, x.value[i], x.value[j]);
            if (UCHAR_MAX - z.value[i+j] < t[0]) {
                carry = 1;
            }
            z.value[i+j] += t[0];
              
            if (UCHAR_MAX - z.value[i+j+1] < t[1]) { // carry 発生
                z.value[i+j+1] += t[1] + carry;
                for (size_t k = 2; i+j+k < z.N; k++) {
                    if (z.value[i+j+k] == UCHAR_MAX) {
                        z.value[i+j+k] = 0;
                    } else {
                        z.value[i+j+k] += 1;
                        break;
                    }
                }
            } else {
                if (carry == 1 && z.value[i+j+1] + t[1] == UCHAR_MAX) {
                    z.value[i+j+1] = 0;
                    for (size_t k = 2; i+j+k < z.N; k++) {
                        if (z.value[i+j+k] == UCHAR_MAX) {
                            z.value[i+j+k] = 0;
                        } else {
                            z.value[i+j+k] += 1;
                            break;
                        }
                    }
                } else {
                    z.value[i+j+1] += t[1] + carry;
                }
            }
        }
    }
    lshift(z.value, z.N, 1);

    for (size_t i = 0; i < x.N; i++) {
        carry = 0;
        mul(t, x.value[i], x.value[i]);
        if (UCHAR_MAX - z.value[2*i] < t[0]) {
            carry = 1;
        }
        z.value[2*i] += t[0];
          
        if (UCHAR_MAX - z.value[2*i+1] < t[1]) { // carry 発生
            z.value[2*i+1] += t[1] + carry;
            for (size_t k = 2; 2*i+k < z.N; k++) {
                if (z.value[2*i+k] == UCHAR_MAX) {
                    z.value[2*i+k] = 0;
                } else {
                    z.value[2*i+k] += 1;
                    break;
                }
            }
        } else {
            if (carry == 1 && z.value[2*i+1] + t[1] == UCHAR_MAX) {
                z.value[2*i+1] = 0;
                for (size_t k = 2; 2*i+k < z.N; k++) {
                    if (z.value[2*i+k] == UCHAR_MAX) {
                        z.value[2*i+k] = 0;
                    } else {
                        z.value[2*i+k] += 1;
                        break;
                    }
                }
            } else {
                z.value[2*i+1] += t[1] + carry;
            }
        }
    }
}
