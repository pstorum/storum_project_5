#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "ctest.h"
#include "image.h"
#include "block.h"
#include "mkfs.h"
#include "inode.h"
#include "free.h"

#define BLOCK_SIZE 4096
#define NUM_BLOCKS 1024
#define NUM_ALLOC_BLOCKS 7
#define BITS_PER_BYTE 8
#define INODE_BLOCK 1
#define FREE_BLOCK 2

#ifdef CTEST_ENABLE


void init_image_fd(void){
    char buffer[BLOCK_SIZE] = {0};
    memset(buffer, 0, sizeof(buffer));
    for(int i = 0; i < NUM_BLOCKS; i++){
        write(image_fd, buffer, sizeof(buffer));
    }
}
//testing image.c
void test_image_open(void){
    char *filename = "test_file";
    int truncate = 1;
    CTEST_ASSERT(image_open(filename, truncate) == 0, "Testing successful file open");
}

void test_image_close(void){
    CTEST_ASSERT(image_close() == 0, "Testing successful file close");
    CTEST_ASSERT(image_close() == -1, "Testing unsuccessful file close");
}

//testing block.c
void test_bread(void){
    char *filename = "test_file";
    int truncate = 1;
    CTEST_ASSERT(image_open(filename, truncate) == 0, "Testing successful file open");
    init_image_fd();
    char buffer[BLOCK_SIZE] = {0};
    unsigned char block[BLOCK_SIZE];
    bread(1, block);
    CTEST_ASSERT(memcmp(buffer, block, BLOCK_SIZE) == 0, "Test if block 1 is filled with all zero's");
    CTEST_ASSERT(image_close() == 0, "Testing successful file close");
}

void test_bwrite(void){
    //image setup
    char *filename = "test_file";
    int truncate = 1;
    CTEST_ASSERT(image_open(filename, truncate) == 0, "Testing successful file open");
    init_image_fd();

    char ones_block[BLOCK_SIZE] = {1};
    unsigned char block[BLOCK_SIZE] = {1};
    unsigned char get_block[BLOCK_SIZE];

    //test successful write.
    bwrite(1, block);
    bread(1, get_block);
    CTEST_ASSERT(memcmp(ones_block, get_block, BLOCK_SIZE) == 0, "Test if block 1 has been written with all 1's");    
    
    //test file close
    CTEST_ASSERT(image_close() == 0, "Testing successful file close");
}

void test_alloc(void){
    //image setup
    char *filename = "test_file";
    int truncate = 1;
    image_open(filename, truncate);
    init_image_fd();

    unsigned char block[BLOCK_SIZE];

    //everything is zero, we can assume first bit to be allocate will be zero
    alloc();
    bread(FREE_BLOCK, block);
    int byte_num = 0 / 8;
    int bit_num = 0 % 8;
    int get_value = (block[byte_num] >> bit_num) & 1;
    CTEST_ASSERT(get_value == 1, "Testing successful of 1st alloc");
    // test for second run of alloc()
    alloc();
    bread(FREE_BLOCK, block);
    byte_num = 1 / 8;
    bit_num = 1 % 8;
    get_value = (block[byte_num] >> bit_num) & 1;
    CTEST_ASSERT(get_value == 1, "Testing successful of 2nd alloc");

    //test of no availible space for allocation
    int alloc_return_value;
    for(int x = 0; x< BITS_PER_BYTE*BLOCK_SIZE; x++){
        alloc_return_value = alloc();
    }
    CTEST_ASSERT(alloc_return_value == -1, "Testing successful of alloc on full block");


    image_close();

}

//testing free.c
void test_set_free(void){
    //image setup
    char *filename = "test_file";
    int truncate = 1;
    image_open(filename, truncate);
    init_image_fd();
    unsigned char block[BLOCK_SIZE];
    bread(1, block);

    //file empty, set any of them
    set_free(block, 6, 1);
    int byte_num = 6 / 8;
    int bit_num = 6 % 8;
    int get_value = (block[byte_num] >> bit_num) & 1;
    CTEST_ASSERT(get_value == 1, "Testing successful of set free");

    //if bit is already 1, should be able to overwrite without issue
    set_free(block, 6, 1);
    get_value = (block[byte_num] >> bit_num) & 1;
    CTEST_ASSERT(get_value == 1, "Testing successful of set free overwrite");

    //free the bit
    set_free(block, 6, 0);
    get_value = (block[byte_num] >> bit_num) & 1;
    CTEST_ASSERT(get_value == 0, "Testing successful of freeing a bit");

    image_close();
}

void test_find_free(void){
    //image setup
    char *filename = "test_file";
    int truncate = 1;
    image_open(filename, truncate);
    init_image_fd();
    unsigned char block[BLOCK_SIZE];
    bread(1, block);

    //find first byte, file is empty should return a 0.
    int byte1 = find_free(block);
    CTEST_ASSERT(byte1 == 0, "Test successful of finding first free");

    //find another byte, when the previous byte has been set as not free
    set_free(block, byte1, 1);
    int byte2 = find_free(block);
    CTEST_ASSERT(byte2 == 1, "Test successful of finding second free");

    image_close();
}

//testing inode.c
void test_ialloc(void){
    //image setup
    char *filename = "test_file";
    int truncate = 1;
    image_open(filename, truncate);
    init_image_fd();

    unsigned char block[BLOCK_SIZE];

    //everything is zero, we can assume first bit to be allocate will be zero
    ialloc();
    bread(INODE_BLOCK, block);
    int byte_num = 0 / 8;
    int bit_num = 0 % 8;
    int get_value = (block[byte_num] >> bit_num) & 1;
    CTEST_ASSERT(get_value == 1, "Testing successful of ialloc");
    // test for second run of ialloc()
    ialloc();
    bread(INODE_BLOCK, block);
    byte_num = 1 / 8;
    bit_num = 1 % 8;
    get_value = (block[byte_num] >> bit_num) & 1;
    CTEST_ASSERT(get_value == 1, "Testing successful of 2nd ialloc");

    //test of no availible space for allocation
    int ialloc_return_value;
    for(int x = 0; x< BITS_PER_BYTE*BLOCK_SIZE; x++){
        ialloc_return_value = ialloc();
    }
    CTEST_ASSERT(ialloc_return_value == -1, "Testing successful of ialloc on full block");


    image_close();
}

//testing mkfs.c
void test_mkfs(void){
    //image setup
    char *filename = "test_file";
    int truncate = 0;
    image_open(filename, truncate);

    char static_block[BLOCK_SIZE] = {0};
    unsigned char block[BLOCK_SIZE];
    
    mkfs();
    //compare static block with all blocks in file except block 2 which has added data from alloc()
    int all_zero = 0;
    for(int i = 0; i < NUM_BLOCKS; i++){
        bread(i, block);
        if(memcmp(static_block, block, BLOCK_SIZE) != 0 && i != 2){
            all_zero = 1;
            break;
        }
    }
    CTEST_ASSERT(all_zero == 0, "Testing File is all zeros but block 2");

    //check block 2 for correct allocation
    unsigned char free_block[BLOCK_SIZE];
    bread(FREE_BLOCK, free_block);
    int first_seven_allocated = 0;
    for(int i = 0; i <NUM_ALLOC_BLOCKS; i++){
        int byte_num = i / 8;
        int bit_num = i % 8;
        int get_value = (free_block[byte_num] >> bit_num) & 1;
        if(get_value != 1){
            first_seven_allocated = 1;
            break;
        }
    }
    CTEST_ASSERT(first_seven_allocated == 0, "Testing Block 2 has first 7 allocated");

    image_close();
}


int main(void){
    CTEST_VERBOSE(1);

    test_image_open();
    test_image_close();
    test_bread();
    test_bwrite();
    test_set_free();
    test_find_free();
    test_ialloc();
    test_alloc();
    test_mkfs();
    
    CTEST_RESULTS();

    CTEST_EXIT();

}
#else

int main(void)
{
    char *filename = "test_file";
    int truncate = 1;
    image_open(filename, truncate);
    image_close();
}

#endif