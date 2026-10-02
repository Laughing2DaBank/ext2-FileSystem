//
// Created by maanas on 02/10/26.
//
#include <stdio.h>
#include "loader.h"

#include <stdlib.h>


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
    return 0;
};

int load_group_desc(ext2_filesystem *fs) {

    uint32_t  numberOfBlockGroups = fs->sb.blocks_count / fs->sb.blocks_per_group;
    if (fs->sb.blocks_count % fs->sb.blocks_per_group != 0) {
        numberOfBlockGroups++;
    }
    fs->gd = malloc(sizeof(ext2_group_desc)*numberOfBlockGroups);

    if (!fs->gd) {
        perror("failed to allocate group descriptor");
        fclose(fs->img);
        return 1;
    }

    if (fseek(fs->img,2048,SEEK_SET) != 0) {
        perror("failed to seek group descriptor table");
        free(fs->gd);
        fclose(fs->img);
        return 1;
    }

    size_t read_count = fread(fs->gd,sizeof(ext2_group_desc),numberOfBlockGroups,fs->img);
    if (read_count != numberOfBlockGroups) {
        perror("failed to read all group descriptor tables\n");
        free(fs->gd);
        fclose(fs->img);
        return 1;
    }

    return 0;
}

