#include <sys/kprintf.h>
#include <stdarg.h>
#include <sys/defs.h>

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
}

void printString(char *s){
    while(*s){
       *temp = *s;
        temp += 2;
		linePos+=2;
CHECK_FOR_FLUSH(*s);
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
        *temp = c;
        temp += 2;
	    linePos+=2;
CHECK_FOR_FLUSH(c);
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
		  }else if(*t == '\r'){
//			*temp = 0xD;
	         temp-=(linePos);
	         linePos = 0;
CHECK_FOR_FLUSH(0);
		  }else{
             *temp = *t;
             temp += 2;
	         linePos+=2;
CHECK_FOR_FLUSH(*t);
         }
   }
}

void kprintf(const char *fmt, ...){
   va_list args;
   va_start(args, fmt);
   parsePrint(fmt, args);
   va_end(args);
}

int32_t puts(char* str)
{
int32_t i = 0;

while(str[i]!='\0')
{
kprintf("%c",str[i]);
i++;
}
return i;
}

void clear_screen()
{
				int i = 0;
				temp = (char *)0xffffffff800b8000;
				for(;i<((160*25)-1);i+=2) 
				{ 
						*((char *)temp + i) = 0; 
				}
				linePos = 0; 
}

