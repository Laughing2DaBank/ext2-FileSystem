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
/*uint32_t allocate_block(ext2_filesystem *fs, bitmap *block_bitmap) {
    for (uint32_t i=0; i<block_bitmap->size; i++) {
        if (!check_bitInUse(block_bitmap, i)) {
            bit_allocate(block_bitmap, i);
            return i;
        }
    }
    return 0;
}*/
/*void write_file(ext2_filesystem *fs, char *path, char *file_name,char *contents ,uint32_t bytes_to_write) {

    uint32_t dir_inode_num = resolve_path(fs,path);
    if (dir_inode_num == 0) {
        fprintf(stderr,"Could not find directory");
        return;
    }

    uint32_t target_inode_num = FindEntry(fs,dir_inode_num,file_name);
    if (target_inode_num == 0) {
        //if file doesnt exist create the file
        fprintf(stderr,"file does not exist");
        return;
    }
    uint32_t group_index = (target_inode_num - 1) / fs->sb.inodes_per_group;
    uint32_t local_index = (target_inode_num - 1) % fs->sb.inodes_per_group;
    ext2_group_desc *group = fs->gd + group_index;

    ext2_inode inode;
    uint32_t inode_offset = (group->starting_block_inodeTable * fs->block_size) + (local_index * fs->inode_size);

    fseek(fs->img, inode_offset, SEEK_SET);
    fread(&inode, sizeof(ext2_inode), 1, fs->img);

    if ((inode.type_perms & 0xF000) != 0x8000) {
        fprintf(stderr,"Error %u is not a file\n",dir_inode_num);
        return;
    }
    if (inode.i_block[0] == 0) {

        //allocate a data block


    }

}*/

void write_to_inode(ext2_filesystem *fs, uint32_t inode_number, const char *contents, uint32_t data_len) {
    uint32_t group_index = (inode_number - 1) / fs->sb.inodes_per_group;
    uint32_t local_index = (inode_number - 1) % fs->sb.inodes_per_group;

    ext2_group_desc *group = fs->gd + group_index;
    uint32_t inode_offset = (group->starting_block_inodeTable * fs->block_size) + (local_index * fs->inode_size);

    ext2_inode inode;
    fseek(fs->img, inode_offset, SEEK_SET);
    fread(&inode, sizeof(ext2_inode), 1, fs->img);

    if ((inode.type_perms & 0xF000) != 0x8000) {
        fprintf(stderr, "Error: Inode %u is not a regular file.\n", inode_number);
        return;
    }

    if (inode.i_block[0] == 0) {
        fprintf(stderr, "Error: Inode %u has no data block allocated yet. (Allocation layer required).\n", inode_number);
        return;
    }

    fseek(fs->img, inode.i_block[0] * fs->block_size, SEEK_SET);
    uint32_t bytes_to_write = (data_len > fs->block_size) ? fs->block_size : data_len;
    fwrite(contents, 1, bytes_to_write, fs->img);

    inode.size = bytes_to_write;
    inode.disk_sector_count = (bytes_to_write + 511) / 512 * 2;

    fseek(fs->img, inode_offset, SEEK_SET);
    fwrite(&inode, sizeof(ext2_inode), 1, fs->img);
    fflush(fs->img);

    printf("Successfully wrote %u bytes to Inode %u (Block %u)\n", bytes_to_write, inode_number, inode.i_block[0]);
}


uint32_t allocate_inode(ext2_filesystem *fs, uint32_t parent_inode_num) {
    uint32_t parent_group_id = (parent_inode_num - 1) / fs->sb.inodes_per_group;
    ext2_group_desc *parent_group = fs->gd + parent_group_id;

    uint32_t inode_bitmap_block = parent_group->inode_bitmap;
    uint8_t *bitmap_buf = malloc(fs->block_size);
    if (!bitmap_buf) return 0;

    fseek(fs->img, inode_bitmap_block * fs->block_size, SEEK_SET);
    fread(bitmap_buf, 1, fs->block_size, fs->img);

    bitmap inode_bitmap;
    inode_bitmap.map = bitmap_buf;
    inode_bitmap.size = fs->sb.inodes_per_group;

    int free_local_index = -1;
    for (uint32_t i = 0; i < fs->sb.inodes_per_group; i++) {
        if (!check_bitInUse(&inode_bitmap, i)) {
            bit_allocate(&inode_bitmap, i);
            free_local_index = i;
            break;
        }
    }

    fseek(fs->img, inode_bitmap_block * fs->block_size, SEEK_SET);
    fwrite(bitmap_buf, 1, fs->block_size, fs->img);
    free(bitmap_buf);

    if (free_local_index == -1) {
        fprintf(stderr, "Error: No free inodes available in group %u\n", parent_group_id);
        return 0;
    }

    uint32_t new_inode_num = (parent_group_id* fs->sb.inodes_per_group) + free_local_index + 1;

    uint32_t inode_table_block = parent_group->starting_block_inodeTable;
    uint32_t inode_offset = (inode_table_block * fs->block_size) + (free_local_index * fs->inode_size);

    ext2_inode new_inode;
    memset(&new_inode, 0, sizeof(ext2_inode));
    new_inode.type_perms = 0x81A4; // S_IFREG | 0644
    new_inode.hard_link_count = 1;
    new_inode.size = 0;

    fseek(fs->img, inode_offset, SEEK_SET);
    fwrite(&new_inode, sizeof(ext2_inode), 1, fs->img);

    fs->sb.free_inodes_count--;
    parent_group->free_inodes_inGroup--;
    fflush(fs->img);

    printf("Allocated new inode %u at disk address offset 0x%X\n", new_inode_num, inode_offset);
    return new_inode_num;
}


void write_buffer_to_disk(ext2_filesystem *fs, uint32_t target_block, uint32_t byte_offset, const char *buffer, uint32_t data_len) {
    uint32_t absolute_position = (target_block * fs->block_size) + byte_offset;

    fseek(fs->img, absolute_position, SEEK_SET);
    fwrite(buffer, 1, data_len, fs->img);
    fflush(fs->img);
}


void append_to_inode(ext2_filesystem *fs, uint32_t inode_number, const char *new_data, uint32_t new_data_len) {
    uint32_t group_index = (inode_number - 1) / fs->sb.inodes_per_group;
    uint32_t local_index = (inode_number - 1) % fs->sb.inodes_per_group;

    ext2_group_desc *group = fs->gd + group_index;
    uint32_t inode_offset = (group->starting_block_inodeTable * fs->block_size) + (local_index * fs->inode_size);

    ext2_inode inode;
    fseek(fs->img, inode_offset, SEEK_SET);
    fread(&inode, sizeof(ext2_inode), 1, fs->img);

    if ((inode.type_perms & 0xF000) != 0x8000) return;

    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    uint32_t existing_size = inode.size;

    if (existing_size > 0 && inode.i_block[0] != 0) {
        fseek(fs->img, inode.i_block[0] * fs->block_size, SEEK_SET);
        fread(buffer, 1, existing_size < sizeof(buffer) ? existing_size : sizeof(buffer) - 1, fs->img);
    }

    if (existing_size + new_data_len >= sizeof(buffer)) return;
    memcpy(buffer + existing_size, new_data, new_data_len);
    uint32_t total_size = existing_size + new_data_len;

    write_to_inode(fs, inode_number, buffer, total_size);
    printf("appended data of size %u bytes\n", total_size);
}


