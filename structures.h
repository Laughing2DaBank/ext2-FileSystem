//
// Created by maanas on 01/10/26.
//

#ifndef EXT2_FILESYSTEM_STRUCTURES_H
#define EXT2_FILESYSTEM_STRUCTURES_H
#include <stdint.h>

typedef struct __attribute__((__packed__)) {
    uint32_t inodes_count;
    uint32_t blocks_count;
    uint32_t reserved_blocks_count;
    uint32_t free_blocks_count;
    uint32_t free_inodes_count;
    uint32_t First_block_containing_superblock;
    uint32_t log_block_size;
    uint32_t log_frag_size;
    uint32_t blocks_per_group;
    uint32_t fragments_per_group;
    uint32_t inodes_per_group;
    uint32_t last_mount_time;
    uint32_t last_write_time;
    uint16_t mount_count;
    uint16_t max_mount_count;
    uint16_t signature;
    uint16_t filesystem_state;
    uint16_t On_error;
    uint16_t Minor_version;
    uint32_t posix_time;
    uint32_t posix_time_interval;
    uint32_t os_id;
    uint32_t major_version;
    uint16_t userId_reserved;
    uint16_t groupId_reserved;

    uint32_t first_non_reserved_inode; //this is 11 by default - ran dumpe2fs and verified it was 11
    uint16_t inode_size;// this is 128 by default but then after running dumpe2fs found out it is actually 256

}ext2_superblock;

typedef enum {
    ext2_error_continue = 1,
    ext2_error_remount_readOnly = 2,
    ext2_error_kernelPanic = 3,
} ext2_error_behavior;

typedef enum {
    ext2_os_linux = 0,
    ext2_os_hurd = 1,
    ext2_os_nasix= 2,
    ext2_os_freebsd = 3,
    ext2_os_lites = 4,
} ext2_os;

typedef enum {
    ext2_valid_fs = 1,
    ext2_error_fs = 2
}ext2_fs_state;

//not implementing the optional feature flags ...

typedef struct __attribute__((__packed__)) {
    uint32_t block_bitmap;
    uint32_t inode_bitmap;
    uint32_t starting_block_inodeTable;
    uint16_t free_blocks_inGroup;
    uint16_t free_inodes_inGroup;
    uint16_t directories_inGroup;

}ext2_block_group;

typedef struct __attribute__((__packed__)) {
    uint32_t inode;
    uint16_t totalSize_entry;
    uint8_t name;
    uint8_t type;
//we need some name characters field here dont know what they mean by N size in bytes
}ext2_directory_entry;

typedef enum {
    unknownType =0,
    regularFile = 1,
    directory = 2,
    characterDevice = 3,
    blockDevice = 4,
    FIFO = 5,
    socket = 6,
    symlink = 7
} directory_entry_type;

typedef struct __attribute__((__packed__)) {
    uint32_t type_perms;
    uint32_t usr_id;
    uint32_t size;
    uint32_t access_time;
    uint32_t create_time;
    uint32_t modification_time;
    uint32_t deletion_time;
    uint16_t group_id;
    uint16_t hard_link_count;
    uint32_t disk_sector_count;
    uint32_t flags;
    uint32_t os_specific;
    //attributes missing


}ext2_inode;

#endif //EXT2_FILESYSTEM_STRUCTURES_H
