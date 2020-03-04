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

    sfsDatabaseSave("file.sfs", db);
    return 0;
}