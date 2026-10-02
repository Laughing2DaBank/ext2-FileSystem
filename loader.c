//
// Created by maanas on 02/10/26.
//
#include <stdio.h>
#include "loader.h"



//bitmap *block_bitmap = NULL;
//bitmap *inode_bitmap = NULL;

/*void filesystem_init() {
    block_bitmap = malloc(sizeof(bitmap));
    inode_bitmap = malloc(sizeof(bitmap));
}*/

int load_superblock(ext2_filesystem *fs) {
    fs->img = fopen(PATH,"rb");
    if (!fs->img) {
        perror("failed to open image file");
        return 1;
    }
    if (fseek(fs->img,1024,SEEK_SET) != 0) {
        perror("failed to seek superblock");
        fclose(fs->img);
        return 1;
    }

    size_t read_count = fread(&(fs->sb),sizeof(fs->sb), 1,fs->img);
    if (read_count != 1) {

        perror("failed to read the full superblock");
        fclose(fs->img);
        return 1;
    }
    if (fs->sb.magic_number != EXT2_SB_SIGNATURE) {
        fprintf(stderr,"bad superblock signature\n");
        fclose(fs->img);
        return 1;
    }
    /*printf("superblock loaded successfully\n");
    printf("superblock signature: %x\n",fs->sb.signature);
    printf("total inode count:%u\n",fs->sb.inodes_count);
    printf("total block count:%u\n",fs->sb.blocks_count);

    uint32_t block_size = 1024 << fs->sb.log_block_size;
    printf("block size:%u\n",block_size);*/


    return 0;

};


