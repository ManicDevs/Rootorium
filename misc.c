#define _GNU_SOURCE

#include <ctype.h>

#include "include/config.h"
#include "include/common.h"
#include "include/misc.h"

char *xorstr(char *string)
{
    int i;
    static char *output;

    for(i = 0; i < strlen(string); i++)
        output[i] ^= 30;

    return output;
}

void trimstr(char *string)
{
    while(*string)
    {
        if(!isprint(*string))
            *string = '\0';
    }
}
