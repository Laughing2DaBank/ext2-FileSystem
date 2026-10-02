//
// Created by maanas on 02/10/26.
//

#ifndef EXT2_FILESYSTEM_LOADER_H
#define EXT2_FILESYSTEM_LOADER_H

#define  PATH "filesystem.img"
#define  EXT2_SB_SIGNATURE 0xEF53


#include <stdio.h>
#include <stdint.h>
#include "structures.h"

typedef struct {
    FILE *img;
    ext2_superblock sb;
    ext2_group_desc *gd;
    uint32_t block_size;
    uint32_t inode_size;
    uint32_t num_groups;
}ext2_filesystem;

int load_superblock(ext2_filesystem *fs);
int load_group_desc(ext2_filesystem *fs);
#endif //EXT2_FILESYSTEM_LOADER_H
