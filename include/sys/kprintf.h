#ifndef __KPRINTF_H
#define __KPRINTF_H
#include<sys/defs.h>
void kprintf(const char *fmt, ...);
int32_t puts(const char* str);
void clear_screen();
#define CHECK_FOR_FLUSH1() \
		do \
{ \
		if(temp > ((char*)(0xffffffff800b8000) + (160*25) - 1)) \
		{ \
				int i = 0;\
				temp = (char *)0xffffffff800b8000;\
				for(;i<((160*25)-1);i+=2) \
				{ \
						*((char *)temp + i) = 0; \
				}\
				linePos = 0; \
		}\
}\
while(0);\



#endif
