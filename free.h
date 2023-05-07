#ifndef FREE_H
#define FREE_H

#define BLOCK_SIZE 4096

unsigned char* set_free(unsigned char *block, int num, int set);
int find_free(unsigned char *block);

#endif