#define _GNU_SOURCE

#include <dlfcn.h>

#include "include/config.h"
#include "include/common.h"
#include "include/dlsym.h"
#include "include/misc.h"

extern void *libc;
extern void *_dl_sym(void *, const char *, void *);

void *(*real_dlsym)(void *handle, const char *name);

void *find_dlsym(void)
{
    int i, maxver = 40;
    char buf[32];

    // Ubuntu
    for(i = 0; i < maxver; i++)
    {
        sprintf(buf, "GLIBC_2.%d", i);
        real_dlsym = (void*(*)(void *handle, const char *name))
            dlvsym(RTLD_NEXT, "dlsym", buf);

        if(real_dlsym)
        {
#ifdef DEBUG
            fprintf(stderr, "Got the real_dlsym!\r\n");
#endif
            return 0;
        }

    }

    // Debian
    for(i = 0; i < maxver; i++)
    {
        int ii;
        for(ii = 0; ii < maxver; ii++)
        {
            sprintf(buf, "GLIBC_2.%d.%d", i, ii);
            real_dlsym = (void*(*)(void *handle, const char *name))
                dlvsym(RTLD_NEXT, "dlsym", buf);

            if(real_dlsym)
            {
#ifdef DEBUG
                fprintf(stderr, "Got the real_dlsym!\r\n");
#endif
                return 0;
            }
        }
    }

    return NULL;
}

void *find_sym(void *handle, char *symbol)
{
    int maxver = 40;
    char buf[32];
    void *func;

    func = dlsym(handle, symbol);
    if(!func)
    {
        int i;
        for(i = 0; i < maxver; i++)
        {
            sprintf(buf, "GLIBC_2.%d", i);
            func = dlvsym(RTLD_NEXT, symbol, buf);
            if(func)
            {
#ifdef DEBUG
                fprintf(stderr, "Got the function: %s!\r\n", symbol);
#endif
                return func;
            }
        }

        for(i = 0; i < maxver; i++)
        {
            int ii;
            for(ii = 0; ii < maxver; ii++)
            {
                sprintf(buf, "GLIBC_2.%d.%d", i, ii);

                func = dlvsym(RTLD_NEXT, symbol, buf);
                if(func)
                {
#ifdef DEBUG
                    fprintf(stderr, "Got the function: %s!\r\n", symbol);
#endif
                    return func;
                }
            }
        }

        if(!func)
            func = _dl_sym(handle, symbol, find_sym);
    }

    return func;
}

void *dlsym(void *handle, const char *name)
{
    void *funcptr;

    if(!libc)
        libc = dlopen(LIBC_PATH, RTLD_LAZY);

    if(!real_dlsym)
        find_dlsym();

#ifdef DEBUG
    fprintf(stderr, "dlsym (%p) debug %s..\r\n", real_dlsym, name);
#endif

    funcptr = real_dlsym(handle, name);

#ifdef DEBUG
    fprintf(stderr, "returned expected %p\r\n", funcptr);
#endif

    return funcptr;
}
