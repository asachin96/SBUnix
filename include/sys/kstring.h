#ifndef _STRING_H
#define _STRING_H

#include <sys/defs.h>

int kstrlen(const char *s);
int kstrcmp(const char *s1, const char *s2);
char* kstrcat(char *str1, const char *str2);
char* kstrcpy(char *dest, const char *src);
char* kstrtok(char *s, const char *delim);
char* kstrcpyn(char *dest, const char *src, uint64_t n);
int32_t atoi (const char * str);
int32_t oct_to_dec(int n);
#endif
