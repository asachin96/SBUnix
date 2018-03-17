#include <stdarg.h>
#include <sys/defs.h>
#include <syscall.h>
#include <stdlib.h>
#include <unistd.h>

void handleArguments(const char* t, va_list*args)
{
								if(*t== 's')
								{
																read(0, (void *) va_arg(*args, char*), 0);
								}
								else if(*t =='d')
								{
																char buff[10]; 
																read(0, buff, 0);
																*((int*) va_arg(*args, int*)) = atoi(buff);
								}
								else if(*t=='c')
								{
																read(0, (void *) va_arg(*args, char*), 0);
								}
}

void scanf(const char *str, ...)
{
								va_list args;
								const char *t = str;
								va_start(args, str);
								for (; *t; t++) {
																if (*t == '%') {
																								t++;
																								handleArguments(t,&args);
																}
								}
								va_end(args); 
}

