#ifndef MKFS_H
#define MKFS_H

#include "image.h"
#include "block.h"

#define BLOCK_SIZE 4096
#define NUM_BLOCKS 1024
#define NUM_ALLOC_BLOCKS 7

void mkfs(void);

#endif