#ifndef INODE_H
#define INODE_H

#include "free.h"
#include "block.h"
#include "image.h"

#define INODE_MAP_BLOCK 1
#define NUM_INODES 1024
#define BLOCK_SIZE 4096
#define SET_ALLOCATED 1

int ialloc(void);

#endif