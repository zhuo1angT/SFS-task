#include "sfs.h"

#include "sfs_process.h"
#include "crc32.h"

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
  *  Construct "SFSVarchar" object in pre-allocated memory space
  */
void sfsVarcharCons(SFSVarchar *varchar, const char* src)
{
    varchar->len = strlen(src);
  
    strcpy(varchar->buf, src); // The function caller is responsible to ensure the buffer size that is enough.
}




/*
 *  Create "SFSVarchar" object using given parameter, return the object
 */
SFSVarchar* sfsVarcharCreate(uint32_t varcharSize, const char* src)
{

    SFSVarchar *ptr = (SFSVarchar *)malloc(sizeof(SFSVarchar) + (varcharSize + 1) * sizeof(char));
   
    ptr->len = varcharSize;
    strcpy(ptr->buf, src);

    return ptr;
}




/*
 *  Free memory space of "SFSVarchar" object.
 */
void sfsVarcharRelease(SFSVarchar *varchar)
{
    free(varchar);
}






 /***********************
 *  SFSTable Functions  *
 ************************/
 
/*
 * (*table) is allocated, its fileds are not.
 */
void sfsTableCons(SFSTable *table, uint32_t initStorSize, const SFSVarchar *recordMeta, SFSDatabase *db)
{
    
    table = (SFSTable *)malloc(sizeof(SFSTable) + getSTLCapacity(initStorSize) * sizeof(char));
    
    table->size = sizeof(SFSTable) + getSTLCapacity(initStorSize) + sizeof(SFSVarchar) + recordMeta->len; 
    table->freeSpace = getSTLCapacity(initStorSize);
    table->storSize = getSTLCapacity(initStorSize);
    table->varcharNum = 0;
    table->recordNum = 0;
    table->recordSize = getStructSize(recordMeta);
    
    table->recordMeta = (SFSVarchar *)recordMeta; // casting from const
    table->lastVarchar = (SFSVarchar *)(table->buf + getSTLCapacity(initStorSize) * sizeof(char) - 1);
    table->database = db;
}


/*
 * Create Table and allocate memory using given parameters.
 */
SFSTable* sfsTableCreate(uint32_t initStorSize, const SFSVarchar *recordMeta, SFSDatabase *db)
{
    
    SFSTable *ptr = (SFSTable *)malloc(sizeof(SFSTable) + getSTLCapacity(initStorSize) * sizeof(char));
    
    // 0x24 bytes of TableHeader, "initStorSize" bytes of storeing space,
    // sizeof(recordMeta) bytes of attached Meta.
    ptr->size = sizeof(SFSTable) + getSTLCapacity(initStorSize) + sizeof(SFSVarchar) + recordMeta->len; 

    ptr->freeSpace = getSTLCapacity(initStorSize);
    ptr->storSize = getSTLCapacity(initStorSize);
        
    ptr->recordSize = getStructSize((SFSVarchar *)recordMeta); // casting from const
    ptr->recordMeta = (SFSVarchar *)recordMeta; // casting from const

    ptr->varcharNum = 0;
    ptr->recordNum = 0;
    
    ptr->lastVarchar = (SFSVarchar *)(ptr->buf + getSTLCapacity(initStorSize) * sizeof(char));
    ptr->database = db;
    
    return ptr;
}


/*
 * Release memory, destroy table object
 */
void sfsTableRelease(SFSTable *table)
{
    free(table->recordMeta);
    free(table);
}


/*
 * After calling this function, it is guaranteed that the Table has at least
 * "storSize" bytes of storSize.
 */ 
void sfsTableReserve(SFSTable **table, uint32_t storSize)
{
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

/*
 * Adding a record to the given table, doubling the storSize when it is necessary.
 */ 
void* sfsTableAddRecord(SFSTable **ptable)
{
    // No Enough Space
    while ((*ptable)->freeSpace < (*ptable)->recordSize){
        sfsTableReserve(ptable, (*ptable)->storSize * 2);
    }
   
    (*ptable)->freeSpace -= (*ptable)->recordSize;
    (*ptable)->recordNum++;

    // Already incremented "recordNum" by one.
    return ((*ptable)->buf) + (*ptable)->recordSize * ((*ptable)->recordNum - 1); 
}


/*
 * Adding a Varchar to the given table, doubling the storSize when it is necessary.
 */
SFSVarchar* sfsTableAddVarchar(SFSTable **ptable, uint32_t varcharLen, const char* src)
{
    // No Enough Space 
    if ((*ptable)->freeSpace < sizeof(SFSVarchar) + strlen(src)){
        // Avoid corner case: Doubling the "storSize", and it still can't hold the string 
        sfsTableReserve(ptable, fmax((*ptable)->storSize * 2, (*ptable)->storSize + varcharLen + 4));
    }

    (*ptable)->freeSpace -= 4 + varcharLen;
    (*ptable)->varcharNum++;

  
    char *tempPtr = (char *)((*ptable)->lastVarchar);   
    tempPtr -= 4 + varcharLen;
    (*ptable)->lastVarchar = (SFSVarchar *)tempPtr;

    
    char *little; // 4 bytes 
    little = intToLittleEndian(varcharLen);
    
    for (int32_t i = 0; i < 4; i++){
        char *writePtr = (char *)((*ptable)->lastVarchar);
        *(writePtr + i) = little[i];
    }
    
    free(little); // It was allocated in function <intToLittleEndian>
        
    strcpy(((char *)((*ptable)->lastVarchar)) + 4, (char *)src); // casting
}





 /**************************
 *  SFSDatabase Functions  *
 **************************/



/*
 * Create a Database, initialize some variables.
 */
SFSDatabase* sfsDatabaseCreate()
{
    SFSDatabase **ptr = (SFSDatabase **)malloc(sizeof(SFSDatabase *)); 
    *ptr = (SFSDatabase *)malloc(sizeof(SFSDatabase));

    (*ptr)->magic = 0x534653aaU;        // "SFS."
    (*ptr)->crc = 0x0U;                 // Just initialize it.
    (*ptr)->version = 1U;               // Just initialize it.
    (*ptr)->size = sizeof(SFSDatabase);
    (*ptr)->tableNum = 0U;

    return *ptr;
}



/*
 * Release memory spaces, destroy the object.
 */ 
void sfsDatabaseRelease(SFSDatabase* db)
{
    for (int32_t i = 0; i < (db->tableNum); i++){
        sfsTableRelease(db->table[i]);
    }
    free(db);
}


/*
 * Save the data to file whose name is filename.
 */ 
void sfsDatabaseSave(char *fileName, SFSDatabase* db)
{


    FILE *file = fopen(fileName, "w");

    printIntToFile(file, db->magic);
    printIntToFile(file, db->crc);
    printIntToFile(file, db->version);
    printIntToFile(file, db->size);
    printCharToFile(file, db->tableNum);
    printCharToFile(file, 0);               
    printCharToFile(file, 0);          // A total of 3 pading bytes to align
    printCharToFile(file, 0);               
    
    // 16 * 4B of table offset
    int accum = 0;
    for (int32_t i = 0; i < 0x10; i++){
        if (i < db->tableNum){
            printIntToFile(file, 84 + accum);
            accum += db->table[i]->size;
        }
        else{
            printIntToFile(file, 0);
        }
    }


    for (int32_t i = 0; i < db->tableNum; i++){

        SFSTable *cur = db->table[i];

        printIntToFile(file, cur->size);
        printIntToFile(file, cur->freeSpace);
        printIntToFile(file, cur->storSize);
        printIntToFile(file, cur->varcharNum);
        printIntToFile(file, cur->recordNum);
        printIntToFile(file, cur->recordSize);
        printIntToFile(file, 36 + cur->storSize);      // Offset to the metadata 
        printIntToFile(file, 36 + ((void *)cur->lastVarchar - (void *)cur->buf)); // Offset to the lastvarchar
        
        // Unclear here
        printIntToFile(file, 0); // Offset to the database 
         

        for (int32_t j = 0; j < cur->storSize; j++){
            printCharToFile(file, cur->buf[j]);
        }
        
        // Metadata
        printIntToFile(file, cur->recordMeta->len);
        for (int32_t j = 0; j < cur->recordMeta->len; j++){
            printCharToFile(file, cur->recordMeta->buf[j]);
        }

    }


    
    fclose(file);


    /* ---- change crc field --- */
    file = fopen(fileName, "rb+");
    
    uint32_t crc32 = CRC_32(file, db->size - 8);
    db->crc = crc32;
    
    fseek(file, 4, SEEK_SET);
    printIntToFile(file, crc32);

    fclose(file);
}


/*
 * Construct a database using data loaded from file
 */ 
SFSDatabase* sfsDatabaseCreateLoad(char *fileName)
{
    
    FILE *file = fopen(fileName, "r");

    if (!CheckValidity(file)){
        fprintf(stderr, "Database validity check failed!\n");
        exit(1);
    }


    SFSDatabase *db = sfsDatabaseCreate();


    fseek(file, 0, SEEK_SET);
    int32_t head = 0;


    LoadIntFromFile(file, &(db->magic));
    LoadIntFromFile(file, &(db->crc));
    LoadIntFromFile(file, &(db->version));
    LoadIntFromFile(file, &(db->size));
    LoadCharFromFile(file, &(db->tableNum));
    LoadCharFromFile(file, db->pad);
    LoadCharFromFile(file, db->pad + 1);        // A total of 3 padding bytes to align
    LoadCharFromFile(file, db->pad + 2);        

    head = 0x14; // head represent cur File read pointer

    for (int32_t i = 0; i < 0x10; i++){
        if (i < db->tableNum){

            int32_t offsetToTable, offsetToMeta, metaLen, offsetTolstVarchar;
            int32_t _;   // drop
            
            LoadIntFromFile(file, &offsetToTable);


            // from begining of the file, offset
            // now the "read-pointer" pointers to the "storSize" of the cur table

            fseek(file, offsetToTable + 8, SEEK_SET); // offset 0x08, in the current table

            int32_t curStorSize;
            SFSVarchar **meta = (SFSVarchar **)malloc(sizeof(SFSVarchar *));
            
            LoadIntFromFile(file, &curStorSize);      // offset 0x0c, in the current table
            fseek(file, 0xc, SEEK_CUR);               // offset 0x18, "offset to the Meta"
            LoadIntFromFile(file, &offsetToMeta);
            fseek(file, -0x1c, SEEK_CUR);             // offset 0x00, the begining of current table
            fseek(file, offsetToMeta, SEEK_CUR);      // Reached the Meta
            LoadIntFromFile(file, &metaLen);          // Reached the begining of "meta.buf"


            *meta = (SFSVarchar *)malloc(sizeof(SFSVarchar) + (size_t)(metaLen));
            
            for (int32_t i = 0; i < metaLen; i++){
                LoadCharFromFile(file, ((char *)((*meta)->buf)) + i);
            }
            
            (*meta)->len = metaLen;
            

            // Got enough data to create a table
            SFSTable *curTable = sfsTableCreate(curStorSize, *meta, db); 
            
            fseek(file, offsetToTable, SEEK_SET); // Go back to Table header
            LoadIntFromFile(file, &(curTable->size));
            LoadIntFromFile(file, &(curTable->freeSpace));
            LoadIntFromFile(file, &(curTable->storSize));
            LoadIntFromFile(file, &(curTable->varcharNum));
            LoadIntFromFile(file, &(curTable->recordNum));
            LoadIntFromFile(file, &(curTable->recordSize)); // 0x18
            LoadIntFromFile(file, &_);                      // drop: the meta have already been set
            LoadIntFromFile(file, &offsetTolstVarchar);
            LoadIntFromFile(file, &_);                      // drop: "database offset"

            curTable->recordMeta = *meta; // actually, it had already been set in "create"

            // copy buf data
            for (int32_t j = 0; j < curTable->storSize; j++){
                uint8_t tempVal;
                LoadCharFromFile(file, &tempVal);//curTable->buf[j]);
                curTable->buf[j] = tempVal;
            }

            curTable->lastVarchar = (SFSVarchar *)curTable->buf + offsetTolstVarchar - 0x24;
            db->table[i] = curTable;

            fseek(file, head, SEEK_SET);
        }
        else{
            int8_t _;   
            LoadCharFromFile(file, &_); // drop
        }
        
        head += 4;
    }


    fclose(file);    
    return db;
}



/*
 * Create and add a table to the given database 
 */ 
SFSTable* sfsDatabaseAddTable(SFSDatabase *db, uint32_t storSize, const SFSVarchar *recordMeta)
{
    assert(db->tableNum <= 0xF);  // A Database can hold at most 0x10 Tables

    SFSTable *newTable = sfsTableCreate(storSize, recordMeta, db);
    
    db->table[db->tableNum] = newTable;
    db->tableNum++;
    db->size += newTable->size;

    return newTable;
}




// Error Report Function
char *sfsErrMsg()
{
    return "";
}
