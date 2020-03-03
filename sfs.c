#include "sfs.h"

#include "sfs_process.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <math.h>



 /* Author: zhuo1ang */



 /***********************
 * SFSVarchar Functions *
 ************************/
 
 /*
  *  <sfsVarcharCons> function is called to construct "SFSVarchar"
  *  object in pre-allocated memory space.
  */
void sfsVarcharCons(SFSVarchar *varchar, const char* src){
    varchar->len = strlen(src);
  
    strcpy(varchar->buf, src); // The function caller is responsible to ensure the buffer size that is enough.
}




/*
 *  <sfsVarcharCreate> function is called to construct "SFSVarchar"
 *  object without allocated memory.
 */
SFSVarchar* sfsVarcharCreate(uint32_t varcharSize, const char* src){

    // In case the pointer to be destroyed by the end of the function 
    // Use malloc to make the pointer allocated on the Heap memory
    
    SFSVarchar **ptr = (SFSVarchar **)malloc(sizeof(SFSVarchar *)); 

    *ptr = (SFSVarchar *)malloc(sizeof(SFSVarchar) + varcharSize * sizeof(char));
    //(*ptr)->buf = (char *)malloc(varcharSize * sizeof(char));
    
    strcpy((*ptr)->buf, src);

    return *ptr;
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
 
/*
 * (*table) is allocated, its fileds are not.
 */
void sfsTableCons(SFSTable *table, uint32_t initStorSize, const SFSVarchar *recordMeta, SFSDatabase *db){
    
    table = (SFSTable *)malloc(sizeof(SFSTable) + initStorSize * sizeof(char));
    
    table->size = sizeof(SFSTable) + initStorSize + sizeof(SFSVarchar) + recordMeta->len; 
    table->freeSpace = initStorSize;
    table->storSize = initStorSize;
    table->varcharNum = 0;
    table->recordNum = 0;
    table->recordSize = getStructSize(recordMeta);
    
    table->recordMeta = (SFSVarchar *)recordMeta; // casting from const
    table->lastVarchar = (SFSVarchar *)(table->buf + initStorSize * sizeof(char) - 1);
    table->database = db;
}


/*
 * 
 */
SFSTable* sfsTableCreate(uint32_t initStorSize, const SFSVarchar *recordMeta, SFSDatabase *db){
    
    SFSTable **ptr = (SFSTable **)malloc(sizeof(SFSTable *)); 
    *ptr = (SFSTable *)malloc(sizeof(SFSTable) + getSTLCapacity(initStorSize) * sizeof(char));
    
    // 0x24 bytes of TableHeader, "initStorSize" bytes of storeing space,
    // sizeof(recordMeta) bytes of attached Meta.
    (*ptr)->size = sizeof(SFSTable) + initStorSize + sizeof(SFSVarchar) + recordMeta->len; 

    (*ptr)->freeSpace = getSTLCapacity(initStorSize) - initStorSize;
    (*ptr)->storSize = getSTLCapacity(initStorSize);
        
    (*ptr)->recordSize = getStructSize((SFSVarchar *)recordMeta); // casting from const
    (*ptr)->recordMeta = (SFSVarchar *)recordMeta; // casting from const

    (*ptr)->varcharNum = initStorSize / (*ptr)->recordSize;
    (*ptr)->recordNum = 0;
    
    (*ptr)->lastVarchar = (SFSVarchar *)((*ptr)->buf + initStorSize * sizeof(char) - 1);
    (*ptr)->database = db;
    
    return *ptr;
}



void sfsTableRelease(SFSTable *table){
    free(table->buf);
    free(table);
}



void sfsTableReserve(SFSTable **table, uint32_t storSize){
    if ((*table)->storSize >= storSize){
        return ;
    }

    SFSTable **ptr = (SFSTable **)malloc(sizeof(SFSTable *));

    int32_t lstPointerOffset = (*ptr)->buf + (*ptr)->storSize - (char *)(*ptr)->lastVarchar;

    *ptr = sfsTableCreate(getSTLCapacity(storSize), (*table)->recordMeta, (*table)->database);
    
    (*ptr)->size = sizeof(SFSTable) + getSTLCapacity(storSize) + sizeof(SFSVarchar) + ((*table)->recordMeta)->len;
    (*ptr)->freeSpace = getSTLCapacity(storSize) - (*table)->storSize;
    (*ptr)->storSize = getSTLCapacity(storSize);

    
    (*ptr)->lastVarchar = (SFSVarchar *)((*ptr)->buf) + (*ptr)->storSize - lstPointerOffset;


    (*ptr)->recordSize = (*table)->recordSize;

    // "recordMeta" have aleady been set in <sfsTableCreate>.

    (*ptr)->varcharNum = (*table)->varcharNum;
    (*ptr)->recordNum = (*table)->recordNum;

    // "db" have aleady been set in <sfsTableCreate>.


    // Copy "records" to the new Table
    memcpy((*ptr)->buf, (*table)->buf, (*table)->recordSize * (*table)->recordNum);
    memcpy((*ptr)->lastVarchar, (*table)->lastVarchar, lstPointerOffset);

    *table = *ptr;
    sfsTableRelease(*table);
}





 /******************************
 *  SFSTable Modify Functions  *
 *******************************/


void* sfsTableAddRecord(SFSTable **ptable){
    // No Enough Space
    while ((*ptable)->freeSpace < (*ptable)->recordSize){
        sfsTableReserve(ptable, (*ptable)->storSize * 2);
    }
   
    (*ptable)->freeSpace -= (*ptable)->recordSize;
    (*ptable)->recordNum++;

    // Already incremented "recordNum" by one.
    return ((*ptable)->buf) + (*ptable)->recordSize * ((*ptable)->recordNum - 1); 
}



SFSVarchar* sfsTableAddVarchar(SFSTable **ptable, uint32_t varcharLen, const char* src){
    // No Enough Space 
    if ((*ptable)->freeSpace < sizeof(SFSVarchar) + strlen(src)){
        // Avoid corner case: Doubling the "storSize", and it still can't hold the string 
        sfsTableReserve(ptable, fmax((*ptable)->storSize * 2, (*ptable)->storSize + varcharLen + 4));
    }

    (*ptable)->lastVarchar -= 4 + sizeof(src);
        
    char little[5], *_; // 4 bytes and a terminating \0
    strcpy(little, _ = intToLittleEndian(varcharLen));
    
    for (int32_t i = 0; i < 4; i++){
        *((char *)((*ptable)->lastVarchar)) = little[i];
    }
    
    free(_); // It was allocated in function <intToLittleEndian>
        
    strcpy((char *)(((*ptable)->lastVarchar) + 4), (char *)src); // casting
}





 /**************************
 *  SFSDatabase Functions  *
 **************************/

SFSDatabase* sfsDatabaseCreate(){
    SFSDatabase **ptr = (SFSDatabase **)malloc(sizeof(SFSDatabase *)); 
    *ptr = (SFSDatabase *)malloc(sizeof(SFSDatabase));

    (*ptr)->magic = 0x534653aaU;        // "SFS."
    (*ptr)->crc = 0x0U;                 // Just initialize it.
    (*ptr)->version = 1U;               // Just initialize it.
    (*ptr)->size = sizeof(SFSDatabase);
    (*ptr)->tableNum = 0U;

    return *ptr;
}




void sfsDatabaseRelease(SFSDatabase* db){
    for (int32_t i = 0; i < (db->tableNum); i++){
        free(db->table[i]);
    }
    free(db);
}




void sfsDatabaseSave(char *fileName, SFSDatabase* db){

    FILE *file;
    file = fopen(fileName, "w");

    printIntToFile(file, db->magic);
    printIntToFile(file, db->crc);
    printIntToFile(file, db->version);
    printIntToFile(file, db->size);
    printCharToFile(file, db->tableNum);
    printCharToFile(file, 0);               
    printCharToFile(file, 0);              // A total of 3 pading bytes to align
    printCharToFile(file, 0);               
    
    // 16 * 4B of table offset
    int accum = 0;
    for (int32_t i = 0; i < 0x10; i++){
        printIntToFile(file, 20 + accum);
        accum += db->table[i]->size;
    }

    accum = 0;
    for (int32_t i = 0; i < db->tableNum; i++){

        SFSTable *cur = db->table[i];

        printIntToFile(file, cur->size);
        printIntToFile(file, cur->freeSpace);
        printIntToFile(file, cur->storSize);
        printIntToFile(file, cur->varcharNum);
        printIntToFile(file, cur->recordNum);
        printIntToFile(file, cur->recordSize);
        printIntToFile(file, accum + 36 + cur->storSize);      // Offset to the metadata 
        printIntToFile(file, accum + 36 + ((void *)cur->lastVarchar - (void *)cur->buf)); // Offset to the lastvarchar
        
        // Unclear here
        printIntToFile(file, accum + 16); // Offset to the database 
         

        for (int32_t j = 0; j < cur->storSize; j++){
            printCharToFile(file, cur->buf[j]);
        }
        
        // Metadata
        printIntToFile(file, cur->recordMeta->len);
        for (int32_t j = 0; j < cur->recordMeta->len; j++){
            printCharToFile(file, cur->recordMeta->buf[j]);
        }

        accum += cur->size;
    }

    fclose(file);
}



SFSDatabase* sfsDatabaseCreateLoad(char *fileName){
    // Todo
}



SFSTable* sfsDatabaseAddTable(SFSDatabase *db, uint32_t storSize, const SFSVarchar *recordMeta){
    assert(db->tableNum <= 0xF);  // A Database can hold at most 0x10 Tables

    SFSTable *newTable = sfsTableCreate(storSize, recordMeta, db);
    db->table[db->tableNum] = newTable;
    db->tableNum++;
    
    return newTable;
}




// Error Report Function

char *sfsErrMsg(){

}