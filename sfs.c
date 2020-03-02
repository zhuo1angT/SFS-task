#include "sfs.h"

#include "process_meta.h"

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
    *ptr = (SFSVarchar *)malloc(sizeof(SFSVarchar));

    (*ptr)->buf = (char *)malloc(varcharSize * sizeof(char));
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
    table->storSize = initStorSize;
    table->recordMeta = recordMeta;
    table->database = db;
    // Todo ?
}


/*
 * 
 */
SFSTable* sfsTableCreate(uint32_t initStorSize, const SFSVarchar *recordMeta, SFSDatabase *db){
    SFSTable **ptr = (SFSTable **)malloc(sizeof(SFSTable *)); 
    *ptr = (SFSTable *)malloc(sizeof(SFSTable));
    
    (*ptr)->storSize = initStorSize;

    (*ptr)->recordSize = 0;
    for (int i = 3; i >= 0; i--){
        (*ptr)->recordSize |= (int)recordMeta[i];
        if (i) (*ptr)->recordSize <<= 8;
    }

    (*ptr)->recordMeta = recordMeta;
    (*ptr)->database = db;
    // Todo
}



int sfsTableRelease(SFSTable *table){
    // Todo
}



int sfsTableReserve(SFSTable **table, uint32_t storSize){
    // Todo
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
    /* (*ptr)->size = 0xffffffffU; */    // Todo
    (*ptr)->tableNum = 0;

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
    assert(db->tableNum < 0xF);
    SFSTable *newTable = sfsTableCreate(storSize, recordMeta, db);
    db->table[db->tableNum] = newTable;
    db->tableNum++;
    /* db->size += ___ */     // Todo
}




// Error Report Function

char *sfsErrMsg(){

}