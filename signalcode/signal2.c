#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define ERR_EXIT(m)                                                                                \
    do                                                                                             \
    {                                                                                              \
        perror(m);                                                                                 \
        exit(EXIT_FAILURE);                                                                        \
    } while(0)
int main() {
    int pipefd[2];
    if(pipe(pipefd) == -1)
        ERR_EXIT("pipe error");

    pid_t pid = fork();
    if(pid == -1)
        ERR_EXIT("fork error");
    if(pid == 0)
    {
        close(pipefd[1]);
        close(pipefd[0]);
        printf("我是子进程，我直接退出了\n");
        exit(EXIT_SUCCESS);
    }
    close(pipefd[0]);
    char buff[16] = "write processs!";
    while(strlen(buff) != 0)
    {
        printf("我是父进程\n");
        write(pipefd[1],buff,strlen(buff));
    }
    close(pipefd[1]);
    return 0;
}