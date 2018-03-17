#include <sys/kprintf.h>
#include <stdarg.h>
#include <sys/defs.h>
#include <sys/idt.h>
#include <sys/util.h>

#define CHECK_FOR_FLUSH(C) \
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
				if(C)\
				{\
						*temp = C;\
						temp+=2;\
						linePos+=2;\
				}\
		}\
}\
while(0);\

char *temp=(char*)0xffffffff800b8000;
int linePos;

void printChar(char c){
    *temp = c;
     temp += 2;
	linePos+=2;
CHECK_FOR_FLUSH(c);
update_cursor();
}

void printString(char *s){
    while(*s){
       *temp = *s;
        temp += 2;
		linePos+=2;
CHECK_FOR_FLUSH(*s);
update_cursor();
        s++;
    }
}

void printInt(int n){
    if(n == 0){
       *temp = '0';
        temp +=2;
        linePos +=2;
    }
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
        *temp = c;
        temp += 2;
	    linePos+=2;
CHECK_FOR_FLUSH(c);
update_cursor();
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
       *temp = c;
       temp += 2;
	   linePos+=2;
CHECK_FOR_FLUSH(c);
update_cursor();
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
       *temp = c;
       temp += 2;
	   linePos+=2;
CHECK_FOR_FLUSH(c);
update_cursor();
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
          }else if(*t == '\n'){
//		      *temp = 0xA;
			   temp+=(160-linePos);
				
	           linePos = 0;
CHECK_FOR_FLUSH(0);
update_cursor();
		  }else if(*t == '\r'){
//			*temp = 0xD;
	         temp-=(linePos);
	         linePos = 0;
CHECK_FOR_FLUSH(0);
update_cursor();
		  }else{
             *temp = *t;
             temp += 2;
	         linePos+=2;
CHECK_FOR_FLUSH(*t);
update_cursor();
         }
   }
}

void kprintf(const char *fmt, ...){
   va_list args;
   va_start(args, fmt);
   parsePrint(fmt, args);
   va_end(args);
}

#pragma GCC push_options
#pragma GCC optimize ("O0")
int puts(const char* str)
{

int volatile i = 0;
while(str[i]!='\0')
{
if(str[i]=='\n'){
 temp+=(160-linePos);
	linePos = 0;
}else{
*temp = str[i];
temp += 2;
linePos+=2;
}
i++;
CHECK_FOR_FLUSH1();
}
update_cursor();
return 1;
}
#pragma GCC pop_options

void clear_screen()
{
				int i = 0;
				temp = (char *)0xffffffff800b8000;
				for(;i<((160*25)-1);i++) 
				{ 
						*((char *)temp + i) = 0; 
				}
				linePos = 0; 
				update_cursor();
}

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
 
	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3E0) & 0xE0) | cursor_end);
}

void disable_cursor()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void update_cursor()
{
  uint16_t cp = (uint16_t)((char*)temp-(char*)0xffffffff800b8000)/2;
  outw(0x3d4, 0x0e | (cp & 0xff00));
  outw(0x3d4, 0x0f | (cp << 8)); 
}

