#include "sfs.h"
#include <stdio.h>

#pragma pack(1)


typedef struct A{
    int8_t x1_1;
    SFSVarchar *x2_v;
    char x3_10[10];
}A;
char AMeta_c[] = {3, 0, 0, 0, 1, 0, 10}; 
SFSVarchar *AMeta = (SFSVarchar *)AMeta_c;
/*  AMeta->len = 3,
    AMeta->buf = {1,0,10} */
const uint32_t ArecordSize = sizeof(A);


int main(){
	SFSDatabase *db = sfsDatabaseCreate();
	SFSTable *table = sfsTableCreate(5 * ArecordSize, AMeta, db);
    A* record = sfsTableAddRecord(&table);
    record->x1_1 = 2;
    record->x2_v = sfsTableAddVarchar(&table, 4, "test");


    db->table[db->tableNum++] = table;
    db->size += table->size;

    sfsDatabaseSave("file.sfs", db);


    /* -------- Loading Test -------- */

    SFSDatabase *loaddb = sfsDatabaseCreateLoad("file.sfs");
    for (int i = 0; i < loaddb->table[0]->storSize; i++){
        printf("%hhu ", loaddb->table[0]->buf[i]);
    }
    
    return 0;
}