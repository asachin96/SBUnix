#include <sys/defs.h>
#include <stdlib.h>
#include <signal.h>
int main(int argc, char* argv[])
{
    pid_t pid;
    int pipefd[2];
    pipe(pipefd);
    char buf;
    if ((pid = fork()) == 0) {
        close(pipefd[1]);
        while(read(pipefd[0], &buf, 1) > 0)
            printf("%c", buf);
        close(pipefd[0]);
    } else {
        close(pipefd[0]);
        write(pipefd[1], "sachin is", 10);
        write(pipefd[1], " great", 6);
        close(pipefd[1]);
        int status;
        waitpid(pid,&status,0);
    }
    return 0;
}

