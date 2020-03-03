#include <stdint.h>
#include "process_meta.h"


uint32_t getFieldNum(char *meta){
    // fieldNum is stored Little-Endian
    uint32_t fieldNum = 0;
    for (uint8_t i = 3; i >= 0; i--){
        fieldNum |= meta[i];
        if (i) fieldNum <<= 8;
    }
    return fieldNum;
}



uint32_t getStructSize(char *meta){
    uint32_t totalSize = 0;
    uint32_t metaLength = strlen(meta); 
    for (uint32_t i = 4; i < metaLength; i++){
        // "meta[i] = 0" means the i-th field is a pointer or (and) array,
        // and the sizeof it was dependent, use "sizeof" operator to get the
        // actual pointer-size.
        totalSize += (meta[i] == 0 ? sizeof(char *) : meta[i]);
    }
    return totalSize;
} 