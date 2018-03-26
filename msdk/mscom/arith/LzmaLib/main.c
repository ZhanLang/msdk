/*
*/

#include "LzmaLite.h"


void cb(char *filename, uint64_t size, void *p);

int main(int argc, char *argv[])
{
    lzmalite_param param;
    param.cb = cb;

    if(argc != 3)
        return -1;

    lzmalite_uncompress(argv[1], argv[2], &param);

    return EXIT_SUCCESS;
}

void cb(char *filename, uint64_t size, void *p)
{

}