# ext2-FileSystem
This is an Implementation of an ext2 File system in C for Web Enthusiasts Club @ NITK 



## The ext2 Filesystem: Philosophy & Design

If you were to design a simple filesystem, you would think about it along these lines - a huge table with every directory entry and its memory address. Nothing is really “wrong” about this approach, it is even fast… unless you maybe rename a directory, then you would have to change / rewrite multiple different rows in your table.

The way your computer handles paths seems so mundane, but then there is a lot of thought that goes into it. Let us break it down.

When we type in `cd ..` - it addresses the previous folder, `cd .` addresses the current folder. You might think the shell resolves every path and traverses through it by some form of string manipulation; let's see if that's the case. 

If it was the case, you would be able to trick the shell by doing something along these lines:
`cd /bin/../filewhichdoesntexist/../bash` — if the shell did do some string manipulation, you would get something along these lines: `cd /bin/bash`. But it actually throws an error! This means that the shell is traversing through every entry in the directory. `..` and `.` are also entries in the directory.

So why does the `ls` command not list it as such? As the command decides to ignore all files starting with a `.` ("dot") — this is how you may create hidden files.

### Directories and Inodes

Now let's unravel the concept of directories being just "files". Each entry in a directory is provided with an inode number, and these inode numbers are actually really significant. These inodes are basically structures which contain all the information about the entry: it could be the last date the file was modified, created, etc., or most importantly, the location they actually exist in memory.

How these inodes are arranged is actually left to the filesystem. Since the filesystem we are going to tackle is an ext2 filesystem kind, these inode structs are stored contiguously, almost like a table of inodes.

This is where most of our logic for reading, writing, and updating comes from.

---

## Filesystem Anatomy (Ext2)

The first 1024 bytes of the filesystem is actually the boot directory; this is probably your MBR/partition where we have your bootloader, etc.

The next 1024 bytes is your **superblock**. The superblock is the heart of the entire filesystem - it contains all the information you would need.

The filesystem is actually broken into smaller block groups. These block groups may contain their own copy of superblocks, inode tables, etc. In case the primary superblock fails to load / gets corrupted, you can use the `fsck -b` command to retrieve the superblock back.

---

## Project Structure & Architecture

Initially the plan was to break the project down into simple digestible files of code to make it easy for me to understand and debug. I've sort of achieved that. The project is divided into 5 major component files:

* **`structures.h`** — This is where all the important structural definitions lie (The Superblock, The Inode, The Directory Entry).
* **`loader.c` / `loader.h`** — This is where the image file is loaded onto the superblock struct.
* **`bitmap.c` / `bitmap.h`** — This is the program file where we allocate / de-allocate blocks, basically anything to do with the inode and block bitmap.
* **`operations.c`** — This is the heart of the entire codebase, where we perform operations such as traversal, reading, writing, and appending.
* **`main.c`** — This is really just the boring menu-driven section of the codebase, but it ties everything together.

---

## Implementation Journey & Milestones

### 1. Loading Superblock and Block Groups
The first main checkpoint in this "journey" was to load the superblock and the block groups. The first 1024 bytes of the filesystem is actually reserved by the boot partition, so all you had to do was seek to position 1024 and load the next 1024 bytes. One interesting thing here is C actually prefers to pad out your structures, so we use `__attribute__((packed))` to make sure C doesn't add any padding — that was really the only hiccup in this part.

Similarly, for block group descriptors which follow right after the superblock in a block descriptors table. After loading in the superblock and the group descriptors, we have completed our first milestone. Now we are ready to tackle directory traversal.

### 2. Directory Traversal & Path Resolution
The heart of directory traversal is the inode and the inode number. I've implemented the function:
`void traverse_directory(ext2_filesystem *fs, uint32_t inode_number)`

`traverse_directory` takes a struct filesystem which contains the image, superblock, group descriptor, block size, inode size, and total number of groups. We always start our traversal journey from the root directory; it is standard that the root directory’s inode is always 2.

**How do we perform traversal?**
* We first find the group’s index which the particular directory lives in.
* Then we find the local inode index (each group has an inode table, so we would need the index of that inode number in its particular table).
* Then we load the group descriptor table (which knows where the inode table exists) for that particular group, and then we load the inode for that particular group.
* You are allotted 12 distinct direct pointers (`i_block`) for each of your inode.
* Scan each of these 12 direct pointers until you encounter an unallocated inode.
* Load whatever directory you see onto a buffer and print it out — this will show you the contents of each directory, that's basically your `ls` command.

**Coming to a change directory command (`cd`):**
It is very similar to the traversal logic, just change your inode number — have a variable that keeps track of what inode number you're currently on. Now if you want to build a `pwd` command, just reverse engineer this process.

### 3. Reading and Writing Logic
Reading and writing also follows a similar principle: load in the inode, have a check if it's a file or a directory (your inode has a type attribute for this), load in your `i_block`s, and then read them. 



There is a little more to it; the logic is something like this:
Keep reading `i_block`s until either you exhaust them, or you need not read any more, or if your `i_block` is empty. The bytes to read can either be a whole `i_block` or it could be a part of it, so we use this simple ternary statement:


`uint32_t bytes_to_read = (bytes_remaining < fs->block_size) ? bytes_remaining : fs->block_size;
bytes_remaining -= bytes_to_read;`

The append command is basically just read + write , we read whatever the file contains , load it onto the buffer then append text into the buffer and dump it 
### Things I've not implemented

1. **Double and Triple Pointer Limits:** The ability to use the double and triple pointers that the inode offers, so the limit to how big you can write your files compared to an actual ext2 filesystem is much smaller.
2. **Relative Directory Tracking (`cd` method):** A simple change directory method which can keep track of your relative directory. Most of the code and implementation is there, but because of lack of time I could not tie it all together (using both the resolve path function and traverse directory function we are more than 90% way there).
3. **Resource Inconsistency:** In theory, you can use up all the inode numbers while barely using any disk space.
4. **Lack of Journaling:** Any unexpected crashes or unclean shutdowns lack journaling support.


### Regarding The Bonuses

What is lock free programming ? 

File locking is basically the mechanism that restricts access to the files by allowing only one user or process to modify or delete it at a specific time 
The way I might approach the bonus questions is basically , we will create a clone of every file you are reading or writing almost like a backup file / buffer file .. we will then let whatever processes and users make changes to the same file - We will create two separate clones and then you as the user get the final say on which clone to keep and which one to delete.


<img width="1259" height="226" alt="Screenshot_20261002_200530" src="https://github.com/user-attachments/assets/f72039e0-b841-47a3-a54d-7b1cc5b1735d" />



<img width="1013" height="291" alt="Screenshot_20261004_155752" src="https://github.com/user-attachments/assets/1c9120ae-2716-4be0-a1f4-b008543a32ad" />



<img width="1259" height="138" alt="Screenshot_20261004_163957" src="https://github.com/user-attachments/assets/52ecd9dd-ea9f-48d3-b1cb-98e35baaa43b" />



<img width="1259" height="226" alt="Screenshot_20261002_200530" src="https://github.com/user-attachments/assets/cfed7568-e155-47a5-8de7-8eaa682de5c6" />


the window on the right is the output verified with dumpe2fs 

<img width="601" height="453" alt="Screenshot_20261005_213804" src="https://github.com/user-attachments/assets/a83d977d-2269-4b1c-aa24-be1bcddf0e83" />



<img width="857" height="362" alt="image" src="https://github.com/user-attachments/assets/7311d872-f0b8-404a-add0-58b51712602e" />


