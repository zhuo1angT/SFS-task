#include "sfs.h"
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

 /***********************
 * SFSVarchar Functions *
 ************************/
 
 /*
  *  <sfsVarcharCons> function is called to construct "SFSVarchar"
  *  object in pre-allocated memory space.
  */
void sfsVarcharCons(SFSVarchar *varchar, const char* src){
    varchar->len = strlen(src);
    strcpy(varchar->buf, src);
}


/*
 *  <sfsVarcharCreate> function is called to construct "SFSVarchar"
 *  object without allocated memory.
 */
SFSVarchar* sfsVarcharCreate(uint32_t varcharSize, const char* src){

    // In case the pointer to be destroyed by the end of the function 
    // Use malloc to make the pointer on the Heap memory
    
    SFSVarchar **Ptr = (SFSVarchar **)malloc(sizeof(SFSVarchar *)); 
    *Ptr = (SFSVarchar *)malloc(sizeof(SFSVarchar));

    (*Ptr)->buf = (char *)malloc(varcharSize * sizeof(char));
    strcpy((*Ptr)->buf, src);

    return *Ptr;
}

/*
 *  <sfsVarcharRelease> function is called to free allocated of "SFSVarchar" object.
 */
void sfsVarcharRelease(SFSVarchar *varchar){
    free(varchar->buf);
    free(varchar);
}


 /***********************
 *  SFSTable Functions  *
 ************************/
 

void sfsTableCons(SFSTable *table, uint32_t initStorSize, const SFSVarchar *recordMeta, SFSDatabase *db){
    table->storSize = initStorSize;
    table->recordMeta = recordMeta;
    table->database = db;
}

SFSTable* sfsTableCreate(uint32_t initStorSize, const SFSVarchar *recordMeta, SFSDatabase *db){
    // Todo
}

int sfsTableRelease(SFSTable *table){
    // Todo
}

int sfsTableReserve(SFSTable **table, uint32_t storSize){
    // Todo
}