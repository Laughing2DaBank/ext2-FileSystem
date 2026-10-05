//
// Created by maanas on 01/10/26.
//
#include <stdio.h>
#include "loader.h"
#include <stdlib.h>
#include <string.h>

#include "operations.h"

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
    fs.block_size = block_size;
    fs.inode_size = 256;
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
   int choice;
    char input_path[256];
    char text[1024];

    // Wrap the entire menu loop so it keeps running
    while (1) {
        printf("\n=== EXT2 Filesystem Menu ===\n");
        printf("1. List Directory Contents\n");
        printf("2. Read File Contents\n");
        printf("3. Write/Overwrite File\n");
        printf("4. Append to File\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input\n");
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');

        switch (choice) {
            case 1: {
                printf("Enter the path to the directory: ");
                if (fgets(input_path, 256, stdin) != NULL) {
                    input_path[strcspn(input_path, "\n")] = 0;
                }
                uint32_t inode_number = resolve_path(&fs, input_path);
                if (inode_number == 0) {
                    printf("Directory not found\n");
                } else {
                    printf("Contents of %s and Inode %u:\n", input_path, inode_number);
                    traverse_directory(&fs, inode_number);
                }
                break;
            }
            case 2: {
                printf("Enter the path to read: ");
                if (fgets(input_path, 256, stdin) != NULL) {
                    input_path[strcspn(input_path, "\n")] = 0;
                    read_file(&fs, input_path);
                    printf("----\n\n");
                }
                break;
            }
            case 3: {
                printf("Enter file path to write/overwrite: ");
                scanf("%s", input_path);
                printf("Enter text to write: ");
                scanf(" %[^\n]", text);

                uint32_t inode_num = resolve_path(&fs, input_path);
                if (inode_num == 0) {
                    char *last_slash = strrchr(input_path, '/');
                    if (last_slash == input_path) {
                        uint32_t parent_inode = resolve_path(&fs, "/");
                        inode_num = allocate_inode(&fs, parent_inode);
                    }
                }

                if (inode_num != 0) {
                    write_to_inode(&fs, inode_num, text, strlen(text));
                }
                break;
            }
            case 4: {
                printf("Enter file path: ");
                scanf("%s", input_path);
                printf("Enter text to append: ");
                scanf(" %[^\n]", text);

                uint32_t inode_num = resolve_path(&fs, input_path);
                if (inode_num == 0) {
                    printf("File not found\n");
                    break;
                }
                append_to_inode(&fs, inode_num, text, strlen(text));
                break;
            }
            case 5: {
                printf("Exiting FileSystem....\n");
                free(fs.gd);
                fclose(fs.img);
                return 0;
            }
            default:
                printf("Invalid choice\n\n");
        }
    }

    free(fs.gd);
    fclose(fs.img);
    return 0;
}
