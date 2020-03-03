#ifndef SFS_PROCESS_H
#define SFS_PROCESS_H

#include <stdint.h>

uint32_t getFieldNum(SFSVarchar *meta);
uint32_t getStructSize(SFSVarchar *meta);

uint32_t getSTLCapacity(uint32_t storSize);

#endif
