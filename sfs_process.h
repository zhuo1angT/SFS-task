#ifndef SFS_PROCESS_H
#define SFS_PROCESS_H

#include <stdint.h>

uint32_t getFieldNum(const SFSVarchar *meta);
uint32_t getStructSize(const SFSVarchar *meta);

uint32_t getSTLCapacity(uint32_t storSize);

// Remember to free!
char * intToLittleEndian(uint32_t len);

#endif
