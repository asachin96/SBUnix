#ifndef __UTIL_H__
#define __UTIL_H__
#define  NULL 0
void outb (unsigned short port, unsigned char data);
unsigned char inb (unsigned short port);
void copyHelper(char* src, char*dest, int offset, int len);
int cStrlen(char *str);
int open (const char* pathname,  int flags);
int read(int fd, void *buf, int count);
int strfind(const char* s1, const char* s2);
char *strchr(const char *s, int c);
char * strtok(char *s, const char* delim);
int cStrCmp(char *str1, char* str2);
void memset(char *str, char val, int size);

#endif
