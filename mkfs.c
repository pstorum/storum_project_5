#include <unistd.h>
#include "mkfs.h"
#include "image.h"
#include "block.h"


void mkfs(void){
    //set all blocks to zero
    char buffer[BLOCK_SIZE] = {0};
    for(int i = 0; i < NUM_BLOCKS; i++){
        write(image_fd, buffer, sizeof(buffer));
    }
    
    //allocate first 7 free blocks
    for(int i = 0; i < NUM_ALLOC_BLOCKS; i++){
        alloc();
    }
}