#include "sfs.h"
#include <string.h>
#include <stdlib.h>
#include <malloc.h>


void sfsVarcharCons(SFSVarchar *varchar, const char* src){
    varchar->len = strlen(src);
    strcpy(varchar->buf, src);
}

SFSVarchar* sfsVarcharCreate(uint32_t varcharSize, const char* src){
    SFSVarchar **Ptr = (SFSVarchar **)malloc(sizeof(SFSVarchar *));
    *Ptr = (SFSVarchar *)malloc(sizeof(SFSVarchar));
    (*Ptr)->buf = (char *)malloc(sizeof(varcharSize));
    strcpy((*Ptr)->buf, src);
    return *Ptr;
}

void sfsVarcharRelease(SFSVarchar *varchar){
    free(varchar->buf);
    free(varchar);
}

