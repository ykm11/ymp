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
    uint8_t *ptr = (uint8_t*)malloc(new_size*sizeof(uint8_t));
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

void dump(const uint8_t v[], size_t n) {
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
    if (v.isNeg) {
        printf("-");
    }    
    printf("0x%X", v.value[v.N-1]);
    for (size_t i = v.N-1; i > 0; i--) {
        printf("%02X", v.value[i-1]);
    }
    puts("");
}

char cmp(const ymp_class &x, const ymp_class &y) {
    if (x.isNeg != y.isNeg) {
        if(x.isNeg) {
            return -1;
        }
        return 1;
    }
    char negFlag = 1;
    if (x.isNeg && y.isNeg) {
        negFlag = -1;
    }

    if (x.N > y.N) {
        return 1*negFlag;
    } else if (x.N < y.N) {
        return -1*negFlag;
    }

    for (size_t i = x.N; i > 0; i--) {
        if (x.value[i-1] > y.value[i-1]) {
            return 1*negFlag;
        } else if(x.value[i-1] < y.value[i-1]) {
            return -1*negFlag;
        }
    }
    return 0;
}

void add(ymp_class &z, const ymp_class &x, const ymp_class &y) {
    // TODO 符号
    uint8_t *tmp = NULL;
    size_t max_index, z_size;
    if (x.N < y.N) {
        max_index = x.N;
        z_size = y.N+1;
        tmp = (uint8_t*)malloc(z_size*sizeof(uint8_t));
        for (size_t i = 0; i < y.N; i++) tmp[i] = y.value[i];
    } else {
        max_index = y.N;
        z_size = x.N+1;
        tmp = (uint8_t*)malloc(z_size*sizeof(uint8_t));
        for (size_t i = 0; i < x.N; i++) tmp[i] = x.value[i];
    }
    if (tmp == NULL) {
        abort();
    }

    uint8_t carry = 0;
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
        for (size_t k = 0; max_index+k < z_size; k++) {
            if (tmp[max_index+k] == UCHAR_MAX) {
                tmp[max_index+k] = 0;
            } else {
                tmp[max_index+k] += 1;
                break;
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

void sub(ymp_class &z, const ymp_class &x, const ymp_class &y) { // z <- x - y
    // TODO 符号
    if (cmp(x, y) < 0) { // x < y
        // 符号情報の変更
        sub(z, y, x);
        ymp_class::neg(z);
        return;
    }

    uint8_t *tmp = NULL;
    uint8_t borrow;
    size_t max_index, z_size;
    if (x.N < y.N) {
        max_index = x.N;
        z_size = y.N;
        tmp = (uint8_t*)malloc(y.N*sizeof(uint8_t));
        for (size_t i = 0; i < y.N; i++) tmp[i] = y.value[i];
    } else {
        max_index = y.N;
        z_size = x.N;
        tmp = (uint8_t*)malloc(x.N*sizeof(uint8_t));
        for (size_t i = 0; i < x.N; i++) tmp[i] = x.value[i];
    }

    if (tmp == NULL) {
        std::cout << "NULLLLLLLLL\n\n";
        abort();
    }

    borrow = 0;
    for (size_t i = 0; i < max_index; i++) {
        if (x.value[i] < y.value[i]) { 
            tmp[i] = x.value[i] - y.value[i] - borrow;
            borrow = 1;
        } else {
            if (x.value[i] - y.value[i] == 0) {
                if (borrow == 1) {
                    tmp[i] = UCHAR_MAX;
                } else {
                    tmp[i] = 0;
                    borrow = 0;
                }
            } else {
                tmp[i] = x.value[i] - y.value[i] - borrow;
                borrow = 0;
            }
        }
    }
    if (borrow == 1) {
        for (size_t k = 0; max_index+k < z.N; k++) {
            if (tmp[max_index+k] == 0) {
                tmp[max_index+k] = UCHAR_MAX;
            } else {
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
    uint8_t *tmp = NULL;
    size_t z_size = x.N + y.N;
    tmp = (uint8_t*)malloc(z_size*sizeof(uint8_t));
    if (tmp == NULL) {
        abort();
    }
    memset(tmp, 0, z_size);
    uint8_t t[2]; 
    uint8_t carry;
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

    z.isNeg = (x.isNeg != y.isNeg);
    z.setSize(z_size);
    for (size_t i = 0; i < z_size; i++) {
        z.value[i] = tmp[i];
    }
    free(tmp);
    removeHeadZero(z, z);
}

void sqr(ymp_class &z, const ymp_class &x) {
    uint8_t *tmp = NULL;
    size_t z_size = x.N*2;
    tmp = (uint8_t*)malloc(z_size*sizeof(uint8_t));
    if (tmp == NULL) {
        abort();
    }
    memset(tmp, 0, z_size);

    uint8_t t[2]; 
    uint8_t carry;
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
    //z.isNeg = false;
    z.setSize(z_size);
    for (size_t i = 0; i < z_size; i++) {
        z.value[i] = tmp[i];
    }
    removeHeadZero(z, z);
    free(tmp);
}

void div(ymp_class &z, const ymp_class &x, const ymp_class &y) { // z = x / y
    /*
    if (cmp(x, y) < 0) {
        z.setSize(1);
        return;
    }
    uint8_t *quot;
    uint8_t *rem;
    quot = (uint8_t *)malloc((x.N - y.N + 1) * sizeof(uint8_t));
    rem = (uint8_t *)malloc(y.N * sizeof(uint8_t));
    x_tmp = (uint8_t *)malloc(x.N * sizeof(uint8_t));
    tmp = (uint8_t *)malloc(x.N * sizeof(uint8_t));
    if (quot == NULL || rem == NULL || x_tmp == NULL || tmp == NULL) {
        abort();
    }
    memset(quot, 0, x.N - y.N + 1);
    memset(rem, 0, y.N);
    memset(tmp, 0, x.N);
    for (size_t i = 0; i < x.N; i++) x_tmp[i] = x.value[i];
    // Quotient

    for (size_t i = 0; i < y.N; i++) {
        tmp[x.N - y.N + i] = y.value[i];
    }
    //

    z.setSize(x.N - y.N + 1);
    for (size_t i = 0; i < z.N; i++) {
        z.value[i] = quot[i];
    }
    removeHeadZero(z, z);
    free(quot);
    free(rem);
    */
}


