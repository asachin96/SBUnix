#ifndef __UTIL_H__
#define __UTIL_H__
#define  NULL 0
#include<sys/defs.h>
#include <sys/procMgr.h>
void outb (unsigned short, unsigned char);
void outw (uint16_t, uint16_t);
unsigned char inb (unsigned short);
int open (const char*,  int);
int read(int, void *, int);
void memset(char*, char, int);
void *memcpy(void*, void*, int) ;
int getNoPages(uint64_t, int);
int atoi(char*);
uint64_t getFlags(uint64_t);
void* kmalloc(int);
int toDecimal(int);
int kstrlen(const char *);
int kstrcmp(const char *, const char *);
char* kstrcat(char *, const char *);
char* kstrcpy(char *, const char *);
char* kstrtok(char *, const char *);
char* kstrcpyn(char *, const char *, int);
#endif
