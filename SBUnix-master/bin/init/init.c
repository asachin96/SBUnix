#include <stdlib.h>

int main(int argc, char **argv)
{
    int status;
    pid_t pid = fork();
    if (pid == 0) {
        execvpe("/rootfs/bin/sh", NULL, NULL);
    } else {
        waitpid(pid, &status, 0);
        printf("shell exited with %d", status);
    }
    return 1;
}

