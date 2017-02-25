#ifndef CONFIG_H
#define CONFIG_H

#define DEBUG 1

#define XORKEY          (uint32_t)0xdeadbeef;

#define LIBC_PATH       "/lib/x86_64-linux-gnu/libc.so.6"

#define MAGIC_STRING    "libc.so.0"
#define MAGIC_GID       197
#define MAGIC_UID       197

#ifdef __i386__
#define BUILD_ARCH      "i686"
#endif

#ifdef __x86_64__
#define BUILD_ARCH      "x86_64"
#endif

#ifdef __arm__
#define BUILD_ARCH      "v6l"
#endif

#endif /* config.h */
