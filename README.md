# ext2-FileSystem
This is an Implementation of an ext2 File system in C for Web Enthusiasts Club @ NITK 


# A Simple Rundown Of The Architecture 

initially the plan was to break the project down into simple digestible files of code to make it easy for me to understand and debug , Ive sort of achieved that 

the project is divided into 5 major components , files rather 
1. structures.h - this is where all the important structural definitions lie (The Superblock , The inode , The directory Entry )
2. loader.c / loader.h - this is where the image file is loaded onto the superblock struct
3. bitmap.c / bitmap.h - this is the program file where we allocate / de-allocate blocks , basically anything to do with the inode and block bitmap 
4. operations.c - this is the heart of the entire codebase , we perform operations such as traversal reading writing and appending
5. main.c - this is really just the boring menu driven section of the codebase but it ties everything together

Coming to how I did implement each of these tasks that were handed to me let me break it down one by one : 

The first main checkpoint in this "journey" was to load the superblock and the block groups . The first 1024 bytes of the filesystem is actually reserved by the boot partition , so all you had to do was seek to position 1024 and load the next 1024 bytes , one interesting thing here is c actually prefers to pad out your structures so we use the attribute(packed) to make sure c doesnt add any padding ~ that was really the only hiccup in this part 

The second milestone was the traversal - traversal looked complicated until it was broken down into multiple pieces 






The logic and theory behind EVERY function in the codebase - 

In an attempt to try to make the entire codebase as modular as possible we have around 15 functions doing different purposes 

The load_superblock(ext2Filesystem *fs) and The load_groupDescriptor(ext2Filesystem *fs) arent really spectacular they do as described and there really isnt much to it except a simple seeking and loading phase 

The real important functions are the ones in the operations.c file 
They are 



void traverse_directory(ext2_filesystem *fs, uint32_t inode_number);
uint32_t resolve_path(ext2_filesystem *fs, char *path);
uint32_t FindEntry(ext2_filesystem *fs,uint32_t dir_inode_num,char *target_name);
void read_file(ext2_filesystem *fs, char *path);
void write_to_inode(ext2_filesystem *fs, uint32_t inode_number, const char *contents, uint32_t data_len);
uint32_t allocate_inode(ext2_filesystem *fs, uint32_t parent_inode_num);
void write_buffer_to_disk(ext2_filesystem *fs, uint32_t target_block, uint32_t byte_offset, const char *buffer, uint32_t data_len);
void append_to_inode(ext2_filesystem *fs, uint32_t inode_number, const char *new_data, uint32_t new_data_len);


It really is gratifying to look and inspect each of these functions at some depth - let us go through them one by one 

the traverse_directory function takes in two parameters the filesystem and the inode number 
--------------------TODO---------------------------------WORKINPROGRESS--------------------------------------------------


As proud as I am about the project that I've built here , there are several shortcomings 
1.The ability to use up all inode values but barely use any of the disk space 
2.Not implemented the double and triple pointer fields the inode blocks have to offer (so the largest file size you can store is minimized by quite a bit)
3.Failure to implement the bonuses as I was short of time
4.Lack of any journaling 
Caveats 1 and 4 existed in the original implementation of the ext2 so hence was born ext3 and ext4 

Here are the outputs - 


<img width="1259" height="226" alt="Screenshot_20261002_200530" src="https://github.com/user-attachments/assets/f72039e0-b841-47a3-a54d-7b1cc5b1735d" />

<img width="1013" height="291" alt="Screenshot_20261004_155752" src="https://github.com/user-attachments/assets/1c9120ae-2716-4be0-a1f4-b008543a32ad" />

<img width="1259" height="138" alt="Screenshot_20261004_163957" src="https://github.com/user-attachments/assets/52ecd9dd-ea9f-48d3-b1cb-98e35baaa43b" />

<img width="1259" height="226" alt="Screenshot_20261002_200530" src="https://github.com/user-attachments/assets/cfed7568-e155-47a5-8de7-8eaa682de5c6" />

the window on the right is the output verified with dumpe2fs 
