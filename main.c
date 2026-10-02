//
// Created by maanas on 01/10/26.
//
#include <stdio.h>
#include "loader.h"


int main(void) {

    ext2_filesystem fs;

    printf("loading filesystem from %s...\n",PATH);

    if (load_superblock(&fs) !=0) {
        fprintf(stderr,"Error loading superblock from %s...\n",PATH);
        return 1;
    }


    printf("superblock loaded successfully\n");
    printf("superblock magic number: %x\n",fs.sb.magic_number);
    printf("total inode count:%u\n",fs.sb.inodes_count);
    printf("total block count:%u\n",fs.sb.blocks_count);

    uint32_t block_size = 1024 << fs.sb.log_block_size;
    printf("block size:%u\n",block_size);
    fclose(fs.img);
    return 0;
}