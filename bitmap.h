//
// Created by maanas on 01/10/26.
//

#ifndef EXT2_FILESYSTEM_BITMAP_H
#define EXT2_FILESYSTEM_BITMAP_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    uint8_t *map;
    uint32_t size;
}bitmap;


bool bitmap_init(bitmap *b,uint32_t total_bits);//implemented
void free_bitmap(bitmap *b);

void bit_allocate(bitmap *b,int bit_num);//implemented
void bit_deallocate(bitmap *b,int bit_num);//implemented
int check_bitInUse(bitmap *b,int bit_num);//implemented

#endif //EXT2_FILESYSTEM_BITMAP_H
