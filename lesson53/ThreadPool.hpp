#pragma once

#include <iostream>
#include <pthread.h>
#include <vector>
#include <queue>
#include "Logger.hpp"
#include "Thread.hpp"
#include "Mutex.hpp"
#include "Cond.hpp"

namespace NS_THREAD_POOL_MODULE {
    using namespace NS_LOG_MODULE;
    using namespace NS_THREAD_MODULE;

    const int defaultnum = 5;
    template <class T>
    class ThreadPool {
    private:
        void HandlerTask() {
            char name[128];
            pthread_getname_np(pthread_self(), name, sizeof(name));
            while(true)
            {
                T task;
                {
                    LockGuard lockguard(_mutex);
                    while(_tasks.empty() && _isrunning)
                    {
                        _slaver_sleep_count++;
                        _cond.Wait(_mutex);
                        _slaver_sleep_count--;
                    }
                    if(!_isrunning && _tasks.empty())
                    {
                        _mutex.Unlock();
                        break;
                    }
                    task = _tasks.front();
                    _tasks.pop();
                }
                LOG(LogLevel::INFO) << name << "处理任务: ";
                task();
            }
            LOG(LogLevel::INFO) << " quit...";
        }
        ThreadPool(int slaver_num = defaultnum)
            : _slaver_num(slaver_num), _isrunning(false), _slaver_sleep_count(0) {
            for(int idx = 0; idx < _slaver_num; idx++)
            {
                _slavers.emplace_back([this]() { this->HandlerTask(); });
            }
        }
        ThreadPool<T> &operator=(const ThreadPool<T> &) = delete;
        ThreadPool(const ThreadPool<T> &) = delete;

    public:
        static ThreadPool<T> *Instance() {
            if(nullptr == _instance)
            {
                LockGuard lockguard(_lock);
                if(nullptr == _instance) // 为什么有两个if判断
                {
                    _instance = new ThreadPool<T>();
                    _instance->Start();
                    LOG(LogLevel::INFO) << "第一次使用线程池， 创建线程池对象";
                }
            }
            return _instance;
        }
        void Start() {
            if(_isrunning)
            {
                LOG(LogLevel::WARNING) << "Thread Pool Is Already Running! ";
                return;
            }
            _isrunning = true;
            for(auto &slaver : _slavers)
            {
                slaver.Start();
            }
        }
        void Stop()
        {
            _mutex.Lock();
            _isrunning = false;
            if(_slaver_sleep_count > 0)
            {
                _cond.Broadcast();
            }
            _mutex.Unlock();
        }
        void Wait()
        {
            for(auto &slaver : _slavers)
            {
                slaver.Join();
            }
        }
        void Enqueue(T in){
            _mutex.Lock();
            _tasks.push(in);
            if(_slaver_sleep_count > 0)
                _cond.Signal();
            _mutex.Unlock();
        }
        ~ThreadPool(){
            
        }
    private:
        bool _isrunning;
        int _slaver_num;
        std::vector<Thread> _slavers;
        std::queue<T> _tasks;
        Mutex _mutex;
        Cond _cond;
        int _slaver_sleep_count;

        static ThreadPool<T> *_instance;
        static Mutex _lock;
    };
    template <typename T>
    ThreadPool<T> *ThreadPool<T>::_instance = nullptr;

    template <typename T>
    Mutex ThreadPool<T>::_lock; // 这个是初始化??
} // namespace NS_THREAD_POOL_MODULE