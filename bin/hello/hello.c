#include <stdio.h>
#include <sys/defs.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void handler(int signo)
{
  //if (signo == 1)
    printf("received SIGINT\n");
}

int main(int argc, char* argv[])
{
    char *buf = (char*)malloc(120);
    /*
    printf("\n Address : %p ", malloc(100));
    char* b = malloc(200);
    printf("\n Address : %p ", b);
    printf("\n Address : %p ", malloc(500));
    free(b);
    printf("\n Address : %p ", malloc(200));
    */
     int fd = open("/rootfs/etc/help", 0);
     if(fd!=-1){
         printf("file opened!!");
         memset(buf, 0,120);
         read(fd, buf, 120);
         printf("reading %s", buf);
     }
     free(buf);
     close(fd);
   /*char buf[300];
   printf("%s",getcwd(buf, 300));
   chdir("/rootfs");
   printf("\n%s",getcwd(buf, 300));
   */
   /*char *c = (char*)malloc(20);
   strcpy(c, "hello");
   free(c);
   printf("%s", c);
   */
//    int i=0;

//   for(;i<4;i++){
     //listprocess();
  // }
   // while(1);
    return 0;
}
