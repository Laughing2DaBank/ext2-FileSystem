//
// Created by maanas on 03/10/26.
//

#include "operations.h"

#include <stdlib.h>
#include <string.h>


void traverse_directory(ext2_filesystem *fs, uint32_t inode_number) {

    uint32_t group_index = (inode_number-1) / fs->sb.inodes_per_group;
    uint32_t local_index = (inode_number-1) % fs->sb.inodes_per_group;

    ext2_group_desc *group = fs->gd + group_index;
    uint32_t inodeTable_block = group->starting_block_inodeTable;

    ext2_inode inode;
    uint32_t inode_offset = (inodeTable_block * fs->block_size) + (local_index*fs->inode_size);

    fseek(fs->img , inode_offset , SEEK_SET);
    fread(&inode,sizeof(ext2_inode),1,fs->img);
    if ((inode.type_perms & 0xF000) != 0x4000) {
        fprintf(stderr,"inode %u is not a directory\n", inode_number);
        return;
    }
    uint8_t *buf = malloc(fs->block_size);
    if (!buf) return;

    for (int i =0; i<12 && inode.i_block[i] != 0; i++) {
        fseek(fs->img,inode.i_block[i] * fs->block_size,SEEK_SET); //check this again
        if (fread(buf,1,fs->block_size,fs->img) != fs->block_size) break;

        uint32_t offset = 0;
        while (offset < fs->block_size) {
            ext2_directory_entry *e = (ext2_directory_entry *)(buf + offset);

            if (e->inode == 0) {

                offset = offset + e->totalSize_entry;
                continue;}
            char name[256];
            int len = e->name_length;
            memcpy(name,e->name,len);
            name[len] = '\0';

            printf(" %s  %u",name, e->inode);

            offset += e->totalSize_entry;
        }

    }
    free(buf);

}

uint32_t resolve_path(ext2_filesystem *fs, char *path) {
    uint32_t current_inode = 2;

    if (strcmp(path,"/")==0) return current_inode;
    char path_copy[1024];
    strcpy(path_copy,path);
    char *token = strtok(path_copy,"/");
    while (token != NULL) {
        current_inode = FindEntry(fs,current_inode,token);
        if (current_inode == 0) {
            return 0;
        }
        token = strtok(NULL,"/");
    }
    return current_inode;
}


uint32_t FindEntry(ext2_filesystem *fs, uint32_t dir_inode_num,char *target_name) {
    // 1. Load the directory inode (similar to your traversal code)
    uint32_t group_index = (dir_inode_num - 1) / fs->sb.inodes_per_group;
    uint32_t local_index = (dir_inode_num - 1) % fs->sb.inodes_per_group;
    ext2_group_desc *group = fs->gd + group_index;

    ext2_inode inode;
    uint32_t inode_offset = (group->starting_block_inodeTable * fs->block_size) + (local_index * fs->inode_size);
    fseek(fs->img, inode_offset, SEEK_SET);
    fread(&inode, sizeof(ext2_inode), 1, fs->img);
    if ((inode.type_perms & 0xF000) != 0x4000) return 0; // Not a directory
    uint8_t *buf = malloc(fs->block_size);
    if (!buf) return 0;
    uint32_t found_inode = 0;

    for (int i = 0; i < 12 && inode.i_block[i] != 0; i++) {
        fseek(fs->img, inode.i_block[i] * fs->block_size, SEEK_SET);
        if (fread(buf, 1, fs->block_size, fs->img) != fs->block_size) break;

        uint32_t offset = 0;
        while (offset < fs->block_size) {
            ext2_directory_entry *e = (ext2_directory_entry *)(buf + offset);
            if (e->inode == 0) {
                offset += e->totalSize_entry;
                continue;
            }
            char name[256];
            int len = e->name_length;
            memcpy(name, e->name, len);
            name[len] = '\0';
            if (strcmp(name, target_name) == 0) {
                found_inode = e->inode;
                break;
            }
            offset += e->totalSize_entry;
        }
        if (found_inode != 0) break;
    }
    free(buf);
    return found_inode;
}


void read_file(ext2_filesystem *fs, char *path) {
    uint32_t inode_number = resolve_path(fs,path);
    if (inode_number == 0) {
        fprintf(stderr,"Could not find inode %u\n",inode_number);
        return;
    }
    uint32_t group_index = (inode_number - 1) / fs->sb.inodes_per_group;
    uint32_t local_index = (inode_number - 1) % fs->sb.inodes_per_group;
    ext2_group_desc *group = fs->gd + group_index;

    ext2_inode inode;
    uint32_t inode_offset = (group->starting_block_inodeTable * fs->block_size) + (local_index * fs->inode_size);

    fseek(fs->img, inode_offset, SEEK_SET);
    fread(&inode, sizeof(ext2_inode), 1, fs->img);

    if ((inode.type_perms & 0xF000) != 0x8000) {
    fprintf(stderr,"Error %u is not a file\n",inode_number);
    return;
    }
    uint32_t bytes_remaining = inode.size;
    uint8_t *buf = malloc(fs->block_size);
    if (!buf) return;

    for (int i=0; i<12 && inode.i_block[i] != 0 && bytes_remaining>0; i++) {
        fseek(fs->img, inode.i_block[i] * fs->block_size, SEEK_SET);
        uint32_t bytes_to_read = (bytes_remaining < fs->block_size) ? bytes_remaining : fs->block_size;
        fread(buf, 1, bytes_to_read, fs->img);
        fwrite(buf, 1, bytes_to_read, stdout);
        bytes_remaining -= bytes_to_read;
    }

    free(buf);

}

uint32_t allocate_block(ext2_filesystem *fs, bitmap *block_bitmap) {
    for (uint32_t i=0; i<block_bitmap->size; i++) {
        if (!check_bitInUse(block_bitmap, i)) {
            bit_allocate(block_bitmap, i);
            return i;
        }
    }
    return 0;
}

