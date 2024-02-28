#include <stdio.h>
#include "string.h"
#include "assert.h"
#include "./argparser.h"
#include <stdlib.h>


struct OPTIONS_STRUCT OPTIONS;

void printUsageAndExit(){
    printf("Usage:\n\t ./hoff3 [decode/code] -i filename -o filename\n");
    printf("\t-i file you want to [code/decode] from\n");
    printf("\t-o file you want to [code/decode] to\n");
    exit(0);
}

struct OPTIONS_STRUCT *parse(int argc, char** argv){
    int necessaryOptions = 3;
    for(int i=0;i<argc;i++){
        
        if(strcmp("decode", argv[i])==0){
            OPTIONS.operation = OP_DECODE;
            necessaryOptions--;
        }
        if(strcmp("code", argv[i])==0){
            OPTIONS.operation = OP_CODE;
            necessaryOptions--;
        }
        if(strcmp("-o", argv[i])==0){
            assert(i<argc-1 && "argumentti puuttuu");
            assert(strlen(argv[i+1]) < FILENAME_BUFF_LEN && "Error: Too long filename");
            strcpy(OPTIONS.outFile, argv[i+1]);
            necessaryOptions--;
        }
        if(strcmp("-i", argv[i])==0){
            assert(i<argc-1 && "argumentti puuttuu");
            assert(strlen(argv[i+1]) < FILENAME_BUFF_LEN && "Error: Too long filename");
            strcpy(OPTIONS.inFile, argv[i+1]);
            necessaryOptions--;
        }
    }
    necessaryOptions != 0 ? printUsageAndExit() : 0;
    return &OPTIONS;
}