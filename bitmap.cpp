//
// Created by maanas on 02/10/26.
//
#include <stdlib.h>
#include "bitmap.h"


bool init_bitmap(bitmap *b , uint32_t total_bits) {
    if (b==NULL) return false;
    uint32_t required_bytes = (total_bits>>3) + ((total_bits&7) ? 1 : 0);
    b->map = (uint8_t *)calloc(required_bytes, sizeof(uint8_t));
    if (b->map==NULL) return false;
    b->size = total_bits;
    return true;
}

