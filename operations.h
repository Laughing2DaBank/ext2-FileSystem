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
#endif //EXT2_FILESYSTEM_TRAVERSAL_H
