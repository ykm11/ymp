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

void ymp_class::neg(ymp_class &z) {
    z.isNeg = !z.isNeg;
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
    unsigned char *tmp = NULL;
    size_t max_index, z_size;
    if (x.N < y.N) {
        max_index = x.N;
        z_size = y.N;
        tmp = (unsigned char*)malloc(y.N*sizeof(unsigned char));
        for (size_t i = 0; i < y.N; i++) tmp[i] = y.value[i];
    } else {
        max_index = y.N;
        z_size = x.N;
        tmp = (unsigned char*)malloc(x.N*sizeof(unsigned char));
        for (size_t i = 0; i < x.N; i++) tmp[i] = x.value[i];
    }
    if (tmp == NULL) {
        abort();
    }

    unsigned char carry = 0;
    for (size_t i = 0; i < max_index; i++) {
        if ((UCHAR_MAX - x.value[i]) < y.value[i]) {
            tmp[i] = x.value[i] + y.value[i] + carry;
            carry = 1;
        } else {
            if (tmp[i] + x.value[i] == UCHAR_MAX) {
                if (carry == 0) {
                    tmp[i] = UCHAR_MAX;
                } else {
                    tmp[i] = 0;
                    carry = 1;
                }
            } else {
                tmp[i] = x.value[i] + y.value[i] + carry;
                carry = 0;
            }
        }
    }
    if (carry == 1) {
        for (size_t k = 0; max_index+k < z.N; k++) {
            if (tmp[max_index+k] == UCHAR_MAX) {
                tmp[max_index+k] = 0;
            } else {
                tmp[max_index+k] += 1;
                break;
            }
        }
    }

    z.setSize(z_size);
    for (size_t i = 0; i < z.N; i++) {
        z.value[i] = tmp[i];
    }
    free(tmp);
}

void sub(ymp_class &z, const ymp_class &x, const ymp_class &y) { // z <- x - y
    if (cmp(x, y) < 0) { // x < y
        // 符号情報の変更
        sub(z, y, x);
        return;
    }

    unsigned char *tmp = NULL;
    unsigned char borrow;
    size_t max_index, z_size;
    if (x.N < y.N) {
        max_index = x.N;
        z_size = y.N;
        tmp = (unsigned char*)malloc(y.N*sizeof(unsigned char));
        for (size_t i = 0; i < y.N; i++) tmp[i] = y.value[i];
    } else {
        max_index = y.N;
        z_size = x.N;
        tmp = (unsigned char*)malloc(x.N*sizeof(unsigned char));
        for (size_t i = 0; i < x.N; i++) tmp[i] = x.value[i];
    }

    if (tmp == NULL) {
        std::cout << "NULLLLLLLLL\n\n";
        abort();
    }

    borrow = 0;
    for (size_t i = 0; i < max_index; i++) {
        if (x.value[i] < y.value[i]) { 
            //z.value[i] = x.value[i] - y.value[i] - borrow;
            tmp[i] = x.value[i] - y.value[i] - borrow;
            borrow = 1;
        } else {
            if (x.value[i] - y.value[i] == 0) {
                if (borrow == 1) {
                    //z.value[i] = UCHAR_MAX;
                    tmp[i] = UCHAR_MAX;
                } else {
                    //z.value[i] = 0;
                    tmp[i] = 0;
                    borrow = 0;
                }
            } else {
                //z.value[i] = x.value[i] - y.value[i] - borrow;
                tmp[i] = x.value[i] - y.value[i] - borrow;
                borrow = 0;
            }
        }
    }
    if (borrow == 1) {
        for (size_t k = 0; max_index+k < z.N; k++) {
            //if (z.value[max_index+k] == 0) {
            if (tmp[max_index+k] == 0) {
                //z.value[max_index+k] = UCHAR_MAX;
                tmp[max_index+k] = UCHAR_MAX;
            } else {
                //z.value[z.N-1] -= 1;
                tmp[z.N-1] -= 1;
                break;
            }
        }
    }

    z.setSize(z_size);
    for (size_t i = 0; i < z.N; i++) {
        z.value[i] = tmp[i];
    }
    free(tmp);
    removeHeadZero(z, z);
}

void mul(ymp_class &z, const ymp_class &x, const ymp_class &y) {
    unsigned char *tmp = NULL;
    size_t z_size = x.N + y.N;
    tmp = (unsigned char*)malloc(z_size*sizeof(unsigned char));
    if (tmp == NULL) {
        abort();
    }
    memset(tmp, 0, z_size);
    unsigned char t[2]; 
    unsigned char carry;
    for (size_t i = 0; i < x.N; i++) {
        for (size_t j = 0; j < y.N; j++) {
            carry = 0;
            mul(t, x.value[i], y.value[j]);
            if (UCHAR_MAX - tmp[i+j] < t[0]) {
                carry = 1;
            }
            tmp[i+j] += t[0];
              
            if (UCHAR_MAX - tmp[i+j+1] < t[1]) { // carry 発生
                tmp[i+j+1] += t[1] + carry;
                for (size_t k = 2; i+j+k < z_size; k++) {
                    if (tmp[i+j+k] == UCHAR_MAX) {
                        tmp[i+j+k] = 0;
                    } else {
                        tmp[i+j+k] += 1;
                        break;
                    }
                }
            } else {
                if (carry == 1 && tmp[i+j+1] + t[1] == UCHAR_MAX) {
                    tmp[i+j+1] = 0;
                    for (size_t k = 2; i+j+k < z_size; k++) {
                        if (tmp[i+j+k] == UCHAR_MAX) {
                            tmp[i+j+k] = 0;
                        } else {
                            tmp[i+j+k] += 1;
                            break;
                        }
                    }
                } else {
                    tmp[i+j+1] += t[1] + carry;
                }
            }
        }
    }

    z.setSize(z_size);
    for (size_t i = 0; i < z_size; i++) {
        z.value[i] = tmp[i];
    }
    free(tmp);
    removeHeadZero(z, z);
}

void sqr(ymp_class &z, const ymp_class &x) {
#if 1
    unsigned char *tmp = NULL;
    size_t z_size = x.N*2;
    tmp = (unsigned char*)malloc(z_size*sizeof(unsigned char));
    if (tmp == NULL) {
        abort();
    }
    memset(tmp, 0, z_size);

    unsigned char t[2]; 
    unsigned char carry;
    for (size_t i = 0; i < x.N; i++) {
        for (size_t j = i+1; j < x.N; j++) {
            carry = 0;
            mul(t, x.value[i], x.value[j]);
            if (UCHAR_MAX - tmp[i+j] < t[0]) {
                carry = 1;
            }
            tmp[i+j] += t[0];
              
            if (UCHAR_MAX - tmp[i+j+1] < t[1]) { // carry 発生
                tmp[i+j+1] += t[1] + carry;
                for (size_t k = 2; i+j+k < z_size; k++) {
                    if (tmp[i+j+k] == UCHAR_MAX) {
                        tmp[i+j+k] = 0;
                    } else {
                        tmp[i+j+k] += 1;
                        break;
                    }
                }
            } else {
                if (carry == 1 && tmp[i+j+1] + t[1] == UCHAR_MAX) {
                    tmp[i+j+1] = 0;
                    for (size_t k = 2; i+j+k < z_size; k++) {
                        if (tmp[i+j+k] == UCHAR_MAX) {
                            tmp[i+j+k] = 0;
                        } else {
                            tmp[i+j+k] += 1;
                            break;
                        }
                    }
                } else {
                    tmp[i+j+1] += t[1] + carry;
                }
            }
        }
    }
    lshift(tmp, z_size, 1);

    for (size_t i = 0; i < x.N; i++) {
        carry = 0;
        mul(t, x.value[i], x.value[i]);
        if (UCHAR_MAX - tmp[2*i] < t[0]) {
            carry = 1;
        }
        tmp[2*i] += t[0];
          
        if (UCHAR_MAX - tmp[2*i+1] < t[1]) { // carry 発生
            tmp[2*i+1] += t[1] + carry;
            for (size_t k = 2; 2*i+k < z_size; k++) {
                if (tmp[2*i+k] == UCHAR_MAX) {
                    tmp[2*i+k] = 0;
                } else {
                    tmp[2*i+k] += 1;
                    break;
                }
            }
        } else {
            if (carry == 1 && tmp[2*i+1] + t[1] == UCHAR_MAX) {
                tmp[2*i+1] = 0;
                for (size_t k = 2; 2*i+k < z_size; k++) {
                    if (tmp[2*i+k] == UCHAR_MAX) {
                        tmp[2*i+k] = 0;
                    } else {
                        tmp[2*i+k] += 1;
                        break;
                    }
                }
            } else {
                tmp[2*i+1] += t[1] + carry;
            }
        }
    }
    z.setSize(z_size);
    for (size_t i = 0; i < z_size; i++) {
        z.value[i] = tmp[i];
    }
    removeHeadZero(z, z);
    free(tmp);
#else
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
    removeHeadZero(z, z);
#endif
}
