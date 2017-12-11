#include <stdarg.h>
#include <sys/defs.h>
#include <sys/common.h>
#include <syscall.h>
#include <stdlib.h>
#include <unistd.h>

static char read_buf[1024];

int handleArguments(const char* ptr,int*len,va_list*ap)
{
           // switch (*++ptr) {
                if(*++ptr== 's')
                    {
																				*len = read(STDIN, read_buf, 0);
                    memcpy((void *) va_arg(*ap, char*), (void *)read_buf, *len); 
																				return 0;
                  //  break;
																				}
             else if(*++ptr =='d')
                {
                    int32_t *dec = (int32_t*) va_arg(*ap, int32_t*);
                    read(STDIN, read_buf, 0);
                    *dec = atoi(read_buf);
                   // break;
																				return 0;
                }
               else if(*++ptr=='c')
                {
                    char *ch = (char *) va_arg(*ap, char*);
                    read(STDIN, read_buf, 0);
                    *ch = read_buf[0]; 
																				return 0;
                }
              //  default:
                //    break;
         //   }
return 0;

}

void scanf(const char *str, ...)
{
    int len;
    va_list ap;
    const char *ptr = NULL;
    va_start(ap, str);
    for (ptr = str; *ptr; ptr++) {
        if (*ptr == '%') {
									if(!handleArguments(ptr,&len,&ap))
											break;
        }
    }
    va_end(ap); 
}

