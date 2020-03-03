#include "sfs.h"

#include "sfs_process.h"

#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>

 /***********************
 * SFSVarchar Functions *
 ************************/
 
 /*
  *  <sfsVarcharCons> function is called to construct "SFSVarchar"
  *  object in pre-allocated memory space.
  */
void sfsVarcharCons(SFSVarchar *varchar, const char* src){
    varchar->len = strlen(src);

    // The function caller is responsible to ensure the buffer size that it is enough.
    strcpy(varchar->buf, src);
}




/*
 *  <sfsVarcharCreate> function is called to construct "SFSVarchar"
 *  object without allocated memory.
 */
SFSVarchar* sfsVarcharCreate(uint32_t varcharSize, const char* src){

    // In case the pointer to be destroyed by the end of the function 
    // Use malloc to make the pointer on the Heap memory
    
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
 * I'm kind of confused... Don't know why it's necessary to have this func.
 */
void sfsTableCons(SFSTable *table, uint32_t initStorSize, const SFSVarchar *recordMeta, SFSDatabase *db){
    if (table->storSize < initStorSize){
        //table->buf = (char *)malloc(initStorSize * sizeof(char));
        table = (SFSTable *)malloc(sizeof(SFSTable) + initStorSize * sizeof(char));
    }
    table->storSize = initStorSize;
    table->recordMeta = (SFSVarchar *)recordMeta; // casting from const
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
    (*ptr)->size = 0x24 + getSTLCapacity(initStorSize) + sizeof(recordMeta);

    (*ptr)->freeSpace = getSTLCapacity(initStorSize) - initStorSize;
    (*ptr)->storSize = getSTLCapacity(initStorSize);
        
    (*ptr)->recordSize = getStructSize((SFSVarchar *)recordMeta); // casting from const
    (*ptr)->recordMeta = (SFSVarchar *)recordMeta; // casting from const

    (*ptr)->varcharNum = initStorSize / (*ptr)->recordSize;
    (*ptr)->recordNum = initStorSize / initStorSize;
    
    
    (*ptr)->database = db;
    
    //(*ptr)->buf = (char *)malloc(getSTLCapacity(initStorSize) * sizeof(char));


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
    *ptr = sfsTableCreate(getSTLCapacity(storSize), (*table)->recordMeta, (*table)->database);
    
    (*ptr)->size = 0x24 + getSTLCapacity(storSize) + sizeof((*table)->recordMeta);

    (*ptr)->freeSpace = (getSTLCapacity(storSize) - (*table)->storSize) + (*table)->freeSpace;
    (*ptr)->storSize = getSTLCapacity(storSize);

    (*ptr)->recordSize = (*table)->recordSize;
    // "recordMeta" have aleady been set in <sfsTableCreate>.

    (*ptr)->varcharNum = (*table)->varcharNum;
    (*ptr)->recordNum = (*table)->recordNum;

    // "db" have aleady been set in <sfsTableCreate>.
    
    //(*ptr)->buf = (char *)malloc(getSTLCapacity(storSize) * sizeof(char));

    *table = *ptr;
}





 /******************************
 *  SFSTable Modify Functions  *
 *******************************/


void* sfsTableAddRecord(SFSTable **ptable){

}


SFSVarchar* sfsTableAddVarchar(SFSTable **ptable, uint32_t varcharLen, const char* src){

}





 /**************************
 *  SFSDatabase Functions  *
 **************************/

SFSDatabase* sfsDatabaseCreate(){
    SFSDatabase **ptr = (SFSDatabase **)malloc(sizeof(SFSDatabase *)); 
    *ptr = (SFSDatabase *)malloc(sizeof(SFSDatabase));

    (*ptr)->magic = 0x534653aaU;
    /* (*ptr)->crc = 0xffffffffU; */     // Todo
    (*ptr)->version = 1U;
    (*ptr)->size = 0U;
    (*ptr)->tableNum = 0U;

    return *ptr;
}




void sfsDatabaseRelease(SFSDatabase* db){
    for (int i = 0; i < (db->tableNum); i++){
        free(db->table[i]);
    }
    free(db);
}




int sfsDatabaseSave(char *fileName, SFSDatabase* db){

}



SFSDatabase* sfsDatabaseCreateLoad(char *fileName){

}



SFSTable* sfsDatabaseAddTable(SFSDatabase *db, uint32_t storSize, const SFSVarchar *recordMeta){
    assert(db->tableNum <= 0xF);
    SFSTable *newTable = sfsTableCreate(storSize, recordMeta, db);
    db->table[db->tableNum] = newTable;
    db->tableNum++;
    /* db->size += ___ */     // Todo
}




// Error Report Function

char *sfsErrMsg(){

}