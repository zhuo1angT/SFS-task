#include "sfs.h"
#include "sfs_process.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

/*
SFSVarchar * genMeta(){
    int fieldNum = rand() % 10 + 1;
    //for (int i = 0; i < )
    return NULL;
}
*/


/* Tested 03/04/14:01 */
void testIntToLE(uint32_t data){
    char *little; // 4 bytes and a terminating \0
    little = intToLittleEndian(data);
    
    for (int i = 0; i < 4; i++)
        if (i != 3) printf("%hhu, ", little[i]);
        else printf("%hhu\n", little[i]);
    free(little);
}

void testIntToLE_main(){
    uint32_t testInput;
    while(scanf("%u", &testInput) != EOF){
        testIntToLE(testInput);
    }
}


bool testPrintIntToFile(int testCaseNum){

    for (int i = 0; i < testCaseNum; i++){
        FILE *writeFile = fopen("testFile.sfs", "w");

        uint32_t data = rand();
        uint32_t orgValue = data;
        //printf("Generate: %u, as unsigned integer to wrire...\n", data);

        printIntToFile(writeFile, data);
        fclose(writeFile);
        
        /*------------------------------------------------*/

        FILE *readFile = fopen("testFile.sfs", "r");
        LoadIntFromFile(readFile, &data);

        //printf("Fetching: %u, from file...\n", data);

        fclose(readFile);
    
        if (data != orgValue){
            fprintf(stderr, "Read Write result doest not correspond!\n");
            fprintf(stderr, "Write %u, bug Load %u\n", orgValue, data);
            return false;
        }
    }
    printf("Success! All %d random cases pass!\n", testCaseNum);
    return true;
}


bool testPrintCharToFile(int testCaseNum){

    for (int i = 0; i < testCaseNum; i++){
        FILE *writeFile = fopen("testFile.sfs", "w");

        uint8_t data = rand() % 256;
        uint8_t orgValue = data;
        
        //printf("Generate: %hhu, as unsigned integer to wrire...\n", data);
        printCharToFile(writeFile, data);
        fclose(writeFile);

        /*------------------------------------------------*/

        FILE *readFile = fopen("testFile.sfs", "r");
        LoadCharFromFile(readFile, &data);
        //printf("Fetching: %hhu, from file...\n", data);
        fclose(readFile);
        
        if (data != orgValue){
            fprintf(stderr, "Read Write result doest not correspond!\n");
            fprintf(stderr, "Write %hhu, bug Load %hhu\n", orgValue, data);
            return false;
        } 
    }

    printf("Success! All %d random cases pass!\n", testCaseNum);
    return true;
}


void testGetSTLCapacity(){
    for (int i = 0; i < 100; i++){
        uint32_t size = rand() % 65536;
        printf("size = %d, STLCapacity = %d\n", size, getSTLCapacity(size));
    }
}


int main(){

    srand(time(NULL));

    // testPrintIntToFile(100000);
    // testPrintCharToFile(100000);
    // testGetSTLCapacity();




    return 0;
}