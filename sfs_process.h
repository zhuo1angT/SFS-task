#ifndef SFS_PROCESS_H
#define SFS_PROCESS_H

#include <stdint.h>

uint32_t getFieldNum(char *meta);
uint32_t getStructSize(char *meta);

uint32_t getSTLCapacity(uint32_t storSize){
    // if storSize is a power of 2
    if (storSize - (storSize & (-storSize) == 0){
        return storSize;
    }
    else{
        while(1){
            int cur = storSize;
            storSize -= (storSize & (-StorSize));
            if (storSize == 0) return cur << 1;
        }
        return 0; /* This statement is not expected to be excecuted. */
    }
}

#endif
