#ifndef DLSYM_H
#define DLSYM_H

void *find_dlsym(void);

void *find_sym(void *handle, char *symbol);

void *dlsym(void *handle, const char *name);

#endif /* dlsym.h */
