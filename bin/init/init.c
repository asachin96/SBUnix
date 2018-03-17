#include <stdlib.h>

int main(int argc, char **argv)
{
    pid_t pid;
    int status;
    if ((pid = fork()) == 0) {
        execvpe("/rootfs/bin/sh", NULL, NULL);
    } else {
        waitpid(pid, &status, 0);
        printf("shell exited with %d", status);
    }
    return 1;
}

