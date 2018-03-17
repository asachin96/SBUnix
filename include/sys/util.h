#ifndef __UTIL_H__
#define __UTIL_H__
#define  NULL 0
#include<sys/defs.h>
void outb (unsigned short port, unsigned char data);
void outw (uint16_t port, uint16_t data);
unsigned char inb (unsigned short port);
void copyHelper(char* src, char*dest, int offset, int len);
int cStrlen(char *str);
int open (const char* pathname,  int flags);
int read(int fd, void *buf, int count);
int strfind(const char* s1, const char* s2);
char *strchr(const char *s, int c);
char * strtok(char *s, const char* delim);
void memset(char *str, char val, int size);
 void *memcpy(void *destination, void *source, uint64_t num) ;

#endif
