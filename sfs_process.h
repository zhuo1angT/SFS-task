#ifndef SFS_PROCESS_H
#define SFS_PROCESS_H

#include "sfs.h"

#include <stdint.h>
#include <stdio.h>

uint32_t getFieldNum(const SFSVarchar *meta);
uint32_t getStructSize(const SFSVarchar *meta);

uint32_t getSTLCapacity(uint32_t storSize);

// Remember to free!
char * intToLittleEndian(uint32_t len);

void printIntToFile(FILE *file, uint32_t data);
void printCharToFile(FILE *file, uint8_t data);

// The function caller has to make sure that it is "the correct place to read"
void LoadIntFromFile(FILE *file, uint32_t *ptr);
void LoadCharFromFile(FILE *file, uint8_t *ptr);


#endif
