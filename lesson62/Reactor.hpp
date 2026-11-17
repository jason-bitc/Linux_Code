#pragma once

#include "Comm.hpp"
#include "Connection.hpp"
#include "Epoller.hpp"
#include "Logger.hpp"
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <sys/epoll.h>
#include <unordered_map>

const static int grevs_num = 64;
class Reactor {
private:
    bool IsLegalConnection(int sockfd) {
        return _connections.find(sockfd) != _connections.end();
    }

public:
    Reactor() : _epoller(std::make_unique<Epoller>()) {}
    void Dispatcher(int n) {
        for(int i = 0; i < n; i++)
        {
            int sockfd = _revs[i].data.fd;
            uint32_t revents = _revs->events;
            if(revents & EPOLLERR)
                revents |= (EPOLLIN | EPOLLOUT);
            else if(revents & EPOLLHUP)
                revents |= (EPOLLIN | EPOLLOUT);
            else if((revents & EPOLLIN) && IsLegalConnection(sockfd))
                _connections[sockfd]->Recver();
            else if((revents & EPOLLOUT) && IsLegalConnection(sockfd))
                _connections[sockfd]->Sender();
        }
    }
    void LoopOnce(int timeout = 1000) {
        int n = _epoller->WaitEvents(_revs, grevs_num, timeout);
        switch(n)
        {
            case 0:
                LOG(LogLevel::INFO) << "Time out...";
                break;
            case -1:
                LOG(LogLevel::FATAL) << "Epoller Wait Error! ";
                exit(EXIT_CODE::EPOLLER_WAIT_FATAL);
                break;
            default:
                LOG(LogLevel::INFO) << "event ready... ";
                Dispatcher(n);
                break;
        }
    }

    void Loop() {
        int timeout = 2000;
        while(true)
        {
            LoopOnce(timeout);
        }
    }
    ~Reactor() {}

private:
    std::unique_ptr<Epoller> _epoller; // 为什么这里会采用智能指针管理呢
    struct epoll_event _revs[grevs_num];
    std::unordered_map<int, std::shared_ptr<Connection>> _connections;
};