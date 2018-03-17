#include <syscall.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/defs.h>

static char write_buf[1024];

char *bufPtr;

void printChar(char c){
								*bufPtr = c;
								bufPtr += 1;
}

void printString(const char *s){
								while(*s){
																*bufPtr = *s;
																bufPtr += 1;
																s++;
								}
}

void printInt(int n){
								int a[10];
								int j=0;
								while(n){
																a[j++] = n%10;
																n = n/10;
								}
								j--;
								while(j>=0){
																char c = a[j] + 48;
																j--;
																*bufPtr = c;
																bufPtr += 1;
								}
}

void printHex(int n){
								int a[10];
								int j=0;
								while(n){
																a[j++] = n%16;
																n = n/16;
								}
								j--;
								while(j>=0){
																char c;
																if(a[j] < 10){
																								c = a[j] + 48;
																}else{
																								c = a[j] + 87;
																}
																j--;
																*bufPtr = c;
																bufPtr += 1;
								}
}

void printPtr(uint64_t n){
								uint64_t a[16];
								int j=0;
								char *str = "0x";
								printString(str);
								while(n){
																a[j++] = n%16;
																n = n/16;
								}
								j--;
								while(j>=0){
																char c;
																if(a[j] < 10){
																								c = a[j] + 48;
																}else{
																								c = a[j] + 87;
																}
																j--;
																*bufPtr = c;
																bufPtr += 1;
								}
}

void parsePrint(const char *fmt, va_list args){
								const char *t = fmt;
								for(;*t;t++){
																if(*t == '%'){
																								t++;
																								switch(*t){
																																case 'c':
																																								printChar(va_arg(args, int));
																																								break;
																																case 's':
																																								printString(va_arg(args, char*));
																																								break;  
																																case 'd':
																																								printInt(va_arg(args, int));
																																								break;
																																case 'x':
																																								printHex(va_arg(args, int));
																																								break;
																																case 'p':
																																								printPtr(va_arg(args, uint64_t));
																																								break;
																																default:
																																								break;
																								}
																}else{
																								*bufPtr = *t;
																								bufPtr += 1;
																}
								}
}

int printf(const char *str, ...)
{
        memset(write_buf, 0, sizeof(write_buf));
        bufPtr = write_buf;
        va_list args;
        va_start(args, str);
        parsePrint(str, args);
        va_end(args);
								write(1, write_buf, strlen(write_buf));
								return strlen(write_buf);
}

int puts(const char *s)
{
  printString(s);
  return 0;
}

int putchar(int c)
{
  printChar(c);
  return c;
}
