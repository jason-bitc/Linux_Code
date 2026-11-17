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
        close(pipefd[0]);
        char *str = "hello,1234567890";
        write(pipefd[1], str, 5);
        // close(pipefd[1]);
        exit(EXIT_SUCCESS);
    }
    close(pipefd[1]);
    char buff[10];
    int ret=0;
    while( (ret = read(pipefd[0], buff, sizeof(buff)-1)) > 0 )
    {
        buff[ret]=0;
        printf("我是父进程，从管道读取到: %s\n", buff);
        printf("read返回:%d\n", ret);
    }
    close(pipefd[0]);
    return 0;
}