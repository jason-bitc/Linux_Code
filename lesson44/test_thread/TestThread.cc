#include <cstddef>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdio>
#include <string>
#include "Task.hpp"
void *Routine(void*)
{

}

int main()
{
    srand(time(nullptr)^getpid());
    const int num = 10;
    for(int i = 0 ; i <  num ; i++)
    {
        int x = rand()%7;
        int y = rand()%8;
        Task task(x,y);
        pthread_t tid;
        int n = pthread_create(&tid, nullptr, Routine, &task);
    }
    

}