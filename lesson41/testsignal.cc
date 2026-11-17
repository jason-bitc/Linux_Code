#include <bits/types/sigset_t.h>
#include <csignal>
#include <iostream>
#include <unistd.h>
#include <signal.h>
void PrintPending (sigset_t &pending)
{
    for(int signo = 31; signo>=1; signo--)
    {
        if(sigismember(&pending, signo))
        {
            std::cout<<"1";
        }else {
            std::cout<<"0";
        }
        std::cout<<std::endl;
    }
}
int main() {
    //1.阻塞2号信号
    sigset_t block_set,old_set;
    sigemptyset(&block_set);
    sigemptyset(&old_set);
    sigaddset(&block_set,SIGINT);
    sigprocmask(SIG_SETMASK, &block_set, &old_set);
    while(true)
    {
        sigset_t pending;
        sigemptyset(&pending);
        sigpending(&pending);
        PrintPending(pending);
        sleep(1);
    }
}