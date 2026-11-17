#pragma once

#include <iostream>
#include <semaphore.h>

class Sem {
public:
    Sem(int init_val) {
        if(init_val >= 0)
        {
            int n = sem_init(&_sem, 0, init_val); // 0的意思？
        }
    }
    void P() {
        int n = sem_wait(&_sem);
    }
    void V() {
        int n = sem_post(&_sem);
    }
    ~Sem() {
        int n = sem_destroy(&_sem);
    }

private:
    sem_t _sem;
};