#ifndef __KPRINTF_H
#define __KPRINTF_H
#include<sys/defs.h>
void kprintf(const char *fmt, ...);
int32_t puts(char* str);
void clear_screen();

#endif
