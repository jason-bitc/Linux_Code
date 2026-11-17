#ifndef __BLOCK_QUEUE_H
#define __BLOCK_QUEUE_H

#include <iostream>
#include <queue>
#include "Mutex.hpp"
#include "Cond.hpp"

const int defaultcap = 5;

template <typename T>
class BlockQueue
{
public:
    BlockQueue(int cap = defaultcap) : _cap(cap)
    {
        sleep_productor_num = 0;
        sleep_consumer_num = 0;
    }
    void Enqueue(T in) // 生产者
    {
        {
            LockGuard lockguard(_mutex);
            // 增强代码的健壮性，鲁棒性，防御性编程
            while (_bq.size() == _cap)
            {
                sleep_productor_num++;
                _productor_cond.Wait(_mutex);
                sleep_productor_num--;
            }

            _bq.push(in);
            if (sleep_consumer_num > 0)
                _consumer_cond.Signal();
        }
    }
    void Pop(T *out) // 消费者
    {
        {
            LockGuard lockguard(_mutex);
            while (_bq.empty())
            {
                sleep_consumer_num++;
                _consumer_cond.Wait(_mutex);
                sleep_consumer_num--;
            }

            *out = _bq.front();
            _bq.pop();

            if (sleep_productor_num > 0)
                _productor_cond.Signal();
        }
    }
    ~BlockQueue()
    {
    }

private:
    std::queue<T> _bq;
    int _cap;

    Mutex _mutex;
    Cond _consumer_cond;
    Cond _productor_cond;

    int sleep_productor_num;
    int sleep_consumer_num;
};

#endif