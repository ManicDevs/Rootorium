#define _GNU_SOURCE

#include <ctype.h>
#include <stdint.h>

#include "include/config.h"
#include "include/common.h"
#include "include/misc.h"

char *xorstr(char *string)
{
    int i, len = strlen(string);
    uint32_t xorkey = XORKEY;

    static char output[] = "";

    uint8_t k1 = xorkey & 0xff,
            k2 = (xorkey >> 8) & 0xff,
            k3 = (xorkey >> 16) & 0xff,
            k4 = (xorkey >> 24) & 0xff;

    for(i = 0; i < len; i++)
    {
        char tmp = string[i] ^ k1;
        tmp ^= k2;
        tmp ^= k3;
        tmp ^= k4;

        output[i] = tmp;
    }

    return output;
}

char *trimstr(char *string)
{
    int i = 0;
    static char *output;

    while(*string)
    {
        if(!isprint(*string))
            *string = '\0';
        output[i++] = *string;
    }

    return output;
}
