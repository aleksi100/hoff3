#ifndef ARGPARSER_H
#define ARGPARSER_H

#define OP_CODE 1
#define OP_DECODE 0
#define FILENAME_BUFF_LEN 64
struct OPTIONS_STRUCT
{
    char inFile[FILENAME_BUFF_LEN];
    char outFile[FILENAME_BUFF_LEN];
    char operation; // OP_DECODE or OP_CODE
};

struct OPTIONS_STRUCT *parse(int argc, char **argv);
#endif