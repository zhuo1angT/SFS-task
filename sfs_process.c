#include <stdint.h>
#include "sfs_process.h"


uint32_t getFieldNum(char *meta){
    // fieldNum is stored Little-Endian
    uint32_t fieldNum = 0;
    for (uint8_t i = 3; i >= 0; i--){
        fieldNum |= meta[i];
        if (i) fieldNum <<= 8;
    }
    return fieldNum;
}



uint32_t getStructSize(SFSVarchar *meta){
    uint32_t totalSize = 0;
    for (uint32_t i = 0; i < meta->len; i++){
        // "meta[i] = 0" means the i-th field is a pointer or (and) array,
        // and the sizeof it was dependent, use "sizeof" operator to get the
        // actual pointer-size.
        totalSize += (meta->buf[i] == 0 ? sizeof(char *) : meta->buf[i]);
    }
    return totalSize;
} 

uint32_t getSTLCapacity(uint32_t storSize){
    // if storSize is a power of 2
    if (storSize - (storSize & (-storSize) == 0)){
        return storSize;
    }
    else{
        while(1){
            int cur = storSize;
            storSize -= (storSize & (-StorSize));
            if (storSize == 0) return cur << 1;
        }
        return 0; /* This line is not expected to be excecuted. */
    }
}


// Remember to free!
char * intToLittleEndian(uint32_t len){
    char little[4] = (char *)malloc(4 * sizeof(char));
    uint8_t byte = 0xAA;
    for (uint32_t i = 0; i < 4; i++){
        little[i] = len & byte;
        little[i] >>= 8;
    }
    return little;
}
