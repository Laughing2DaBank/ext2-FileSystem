//
// Created by maanas on 01/10/26.
//
#include <stdio.h>
#include "loader.h"
#include <stdlib.h>

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

    if (load_group_desc(&fs) !=0) {
        fprintf(stderr,"Error loading group descriptor from %s...\n",PATH);
    }
    printf("group descriptor loaded successfully\n");

    uint32_t  numberOfBlockGroups = fs.sb.blocks_count / fs.sb.blocks_per_group;
    if (fs.sb.blocks_count % fs.sb.blocks_per_group != 0) {
        numberOfBlockGroups++;
    }
    printf("\n ------ %u blocks ------\n",numberOfBlockGroups);
    for (uint32_t i = 0; i < numberOfBlockGroups; i++) {
        printf("-------------------\n");
        printf("block bitmap %u\n",fs.gd[i].block_bitmap);
        printf("inode bitmap %u\n",fs.gd[i].inode_bitmap);
        printf("starting block in the inode table %u\n",fs.gd[i].starting_block_inodeTable);
        printf("free blocks %u\n",fs.gd[i].free_blocks_inGroup);
        printf("free inodes %u\n",fs.gd[i].free_inodes_inGroup);
        printf("used directories %u\n",fs.gd[i].directories_inGroup);
    }

    free(fs.gd);
    fclose(fs.img);
    return 0;
}