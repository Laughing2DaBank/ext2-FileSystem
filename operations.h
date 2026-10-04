//
// Created by maanas on 03/10/26.
//

#ifndef EXT2_FILESYSTEM_TRAVERSAL_H
#define EXT2_FILESYSTEM_TRAVERSAL_H

#include <stdio.h>
#include <stdint.h>
#include "structures.h"
#include "loader.h"
#include "bitmap.h"


void traverse_directory(ext2_filesystem *fs, uint32_t inode_number);
uint32_t resolve_path(ext2_filesystem *fs, char *path);
uint32_t FindEntry(ext2_filesystem *fs,uint32_t dir_inode_num,char *target_name);
void read_file(ext2_filesystem *fs, char *path);
void write_to_inode(ext2_filesystem *fs, uint32_t inode_number, const char *contents, uint32_t data_len);
uint32_t allocate_inode(ext2_filesystem *fs, uint32_t parent_inode_num);
void write_buffer_to_disk(ext2_filesystem *fs, uint32_t target_block, uint32_t byte_offset, const char *buffer, uint32_t data_len);
void append_to_inode(ext2_filesystem *fs, uint32_t inode_number, const char *new_data, uint32_t new_data_len);

#endif //EXT2_FILESYSTEM_TRAVERSAL_H
