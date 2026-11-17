#pragma once

#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>

// 1：重定向, 1:更改
void Daemon(int isdup, int isredir)
{
    // 1. 忽略信号
    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);

    // 2. 不能是组长
    if (fork() > 0)
        exit(0); // 父进程

    // 3. 设置新会话
    pid_t id = setsid();
    (void)id;

    // 4. 更改守护进程的工作路径--??
    if (isredir)
        chdir("/");

    // 5. 重定向0,1,2 -- ??
    if (isdup)
    {
        close(0);
        close(1);
        close(2);
    }
    else
    {

        int fd = open("/dev/null", O_RDWR);
        if (fd >= 0)
        {
            dup2(fd, 0);
            dup2(fd, 1);
            dup2(fd, 2);
            close(fd);
        }
    }
}