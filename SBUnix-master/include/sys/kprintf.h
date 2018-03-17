#ifndef __KPRINTF_H
#define __KPRINTF_H
#include<sys/defs.h>
void kprintf(const char *fmt, ...);
int puts(const char* str);
void clear_screen();
void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void disable_cursor();
void update_cursor();
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
