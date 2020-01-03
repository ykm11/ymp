#include "miscCalc.hpp"
#include <stdint.h>

#include <iostream>

void mul(uint8_t z[], uint8_t x0, uint8_t y0) {
#if 0
    uint8_t z0, z2;
    uint8_t x1;
    uint8_t y1;
    uint8_t carry0, carry1, tmp;
    uint16_t z1;
    x1 = (x0 >> 4);
    y1 = (y0 >> 4);
    x0 = x0 & 0x0F;
    y0 = y0 & 0x0F;

    z2 = x1 * y1;
    z0 = x0 * y0;
    tmp = -(x0-x1)*(y0-y1);
    z1 = z2 + z0 + tmp;
    //printf("z1: %X\n", z1); 
    //printf("tmp: %d\n", tmp >> 7); 
    if ((tmp >> 7) == 1) {
        //z1 ^= 0x100;
        z1 = z1 & 0xFF;
    }

    z[1] = z2;
    z[0] = z0;

    if ((UCHAR_MAX - z[0]) > ((z1 & 0x0F) << 4)) {
        carry1 = 0;
    } else {
        carry1 = 1;
    }
    z[0] = z[0] + ((z1 & 0x0F) << 4);
    z[1] = z[1] + (z1 >> 4) + carry1;
#else
    uint16_t z1;
    z1 = x0 * y0;
    z[0] = z1 & 0xFF;
    z[1] = (z1 >> 8) & 0xFF;
#endif
}

