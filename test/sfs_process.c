#include "sfs_process.h"
#include "sfs.h"

#include <stdint.h>
#include <malloc.h>


uint32_t getFieldNum(const SFSVarchar *meta)
{
    // fieldNum is stored Little-Endian
    uint32_t fieldNum = 0;
    for (uint8_t i = 3; i >= 0; i--){
        fieldNum |= ((char *)meta)[i];
        if (i) fieldNum <<= 8;
    }
    return fieldNum;
}



uint32_t getStructSize(const SFSVarchar *meta)
{
    uint32_t totalSize = 0;
    for (uint32_t i = 0; i < meta->len; i++){
        // "meta[i] = 0" means the i-th field is a pointer or (and) array,
        // and the sizeof it was dependent, use "sizeof" operator to get the
        // actual pointer-size.
        totalSize += (meta->buf[i] == 0 ? sizeof(char *) : meta->buf[i]);
    }
    return totalSize;
} 


// Tested 03/04/15:03
uint32_t getSTLCapacity(uint32_t storSize)
{
    // if storSize is a power of 2
    if (storSize - (storSize & (-storSize)) == 0){
        return storSize;
    }
    else{
        while(1){
            int cur = storSize;
            storSize -= (storSize & (-storSize));
            if (storSize == 0) return cur << 1;
        }
        return 0; /* This line is not expected to be excecuted. */
    }
}





// Tested 03/04/14:01     // Remember to free!
char * intToLittleEndian(uint32_t len)
{
    char *little = (char *)malloc(4 * sizeof(char));
    uint8_t byte = 0xFF;
    for (uint32_t i = 0; i < 4; i++){
        little[i] = len & byte;
        len >>= 8;
    }
    return little;
}



// Tested 03/04/14:47
void printIntToFile(FILE *file, uint32_t data)
{
    uint8_t byte = 0xFF;
    for (uint32_t i = 0; i < 4; i++){
        fprintf(file, "%c", (uint8_t)(data & byte));
        data >>= 8;
    }
}

// Tested 03/04/14:47
void printCharToFile(FILE *file, uint8_t data)
{
    fprintf(file, "%c", data);
}


// Tested 03/04/14:47
void LoadIntFromFile(FILE *file, uint32_t *ptr)
{
    uint32_t data = 0x0U;
    uint8_t value;
    for (uint32_t i = 0; i < 4; i++){
        fscanf(file, "%c", &value);
        data |= (value << (i * 8));
    }
    *ptr = data;
}

// Tested 03/04/14:47
void LoadCharFromFile(FILE *file, uint8_t *ptr)
{
    uint8_t data;
    fscanf(file, "%c", &data);
    *ptr = data;
}