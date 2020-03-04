#ifndef CRC32_H
#define CRC32_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

uint32_t CRC_32(FILE *file, uint32_t len);

bool CheckValidity(FILE *file);

#endif