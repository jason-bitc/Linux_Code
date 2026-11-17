#pragma once
#include <cstddef>
#include <pthread.h>
#include <queue>
#include <iostream>
#include <unistd.h>
#include <vector>

const int defaultcap = 5;
template <class T>
class BlockQueue {
public:
    BlockQueue(int cap = defaultcap) : _cap(cap) {
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_consumer_cond, nullptr);
        pthread_cond_init(&_productor_cond, nullptr);

        sleep_productor_num = 0;
        sleep_consumer_num = 0;
    }
    void Enqueue(T &in) {
        pthread_mutex_lock(&_mutex);
        while(_bq.size() == _cap)//??>=
        {
            sleep_productor_num++;
            pthread_cond_wait(&_productor_cond,&_mutex);
            sleep_productor_num--;
        }
        _bq.push(in);
        if(sleep_consumer_num > 0)
            pthread_cond_signal(&_consumer_cond);
        pthread_mutex_unlock(&_mutex);
    }
    void Pop(T* out)
    {
        pthread_mutex_lock(&_mutex);
        while(_bq.empty())
        {
            sleep_consumer_num++;
            pthread_cond_wait(&_consumer_cond,&_mutex);
            sleep_consumer_num--;
        }
        *out = _bq.front();
        _bq.pop();
        if(sleep_productor_num > 0)
            pthread_cond_signal(&_productor_cond);
        pthread_mutex_unlock(&_mutex);
    }

    ~BlockQueue() {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_consumer_cond);
        pthread_cond_destroy(&_productor_cond);
    }

private:
    std::queue<T> _bq;
    int _cap;

    pthread_mutex_t _mutex;
    pthread_cond_t _consumer_cond;
    pthread_cond_t _productor_cond;

    int sleep_productor_num;
    int sleep_consumer_num;
};