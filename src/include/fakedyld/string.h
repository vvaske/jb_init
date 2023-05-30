#ifndef FAKEDYLD_STRING_H
#define FAKEDYLD_STRING_H

#include <stddef.h>
extern const char* const sys_errlist[];

size_t strlen(const char* str);
char *strstr(const char *string, char *substring);
char *strchr(const char *p, int ch);

const char* strerror(int err);
#endif
