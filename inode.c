#include "inode.h"

int ialloc(void){

    unsigned char inode_map[BLOCK_SIZE];
    int inode_num;
    
    //read in the block
    bread(INODE_MAP_BLOCK, inode_map);
    
    //find free, if none are free return -1
    inode_num = find_free(inode_map);
    if (inode_num == -1) {
        return -1;
    }

    //set as allocated and write
    set_free(inode_map, inode_num, SET_ALLOCATED);    
    bwrite(INODE_MAP_BLOCK, inode_map);
    
    return inode_num;
}