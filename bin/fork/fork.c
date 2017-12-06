#include <sys/defs.h>
#include <stdlib.h>

char buf[10] = "FORK";

int main(int argc, char* argv[])
{
    pid_t pid;
    printf("\nProcess");

//    pid = fork();
//    printf("\nFork return: %d Process %d (parent %d)", pid, getpid(), getppid());
    if ((pid = fork()) == 0) {
        printf("child");
    } else {
        printf("parent");
    }

    while(1);
    return 0;
}

