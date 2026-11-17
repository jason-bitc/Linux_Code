#pragma once

#include <iostream>
#include <sys/epoll.h>
#include <cstdlib>
#include "Comm.hpp"
#include "Logger.hpp"

using namespace NS_LOG_MODULE;

class Epoller {
private:
    bool EpollCtl(int sockfd, int oper, uint32_t events) {
        if(oper == EPOLL_CTL_DEL)
        {
            int n = epoll_ctl(_epfd, oper, sockfd, nullptr);
        }
        else
        {
            struct epoll_event ev;
            ev.events = events;
            ev.data.fd = sockfd;
            int m = epoll_ctl(_epfd, oper, sockfd, &ev);
        }
        return true;
    }

public:
    Epoller() : _epfd(-1) {
        _epfd = epoll_create(256);
        if(_epfd < 0)
        {
            LOG(LogLevel::FATAL) << "Epoll Create Error! ";
            exit(EXIT_CODE::EPOLLER_ERROR);
        }
        LOG(LogLevel::INFO) << "Epoll Create Success! ";
    }
    int WaitEvents(struct epoll_event revs[], int eventnum, int timeout) {
        return epoll_wait(_epfd, revs, eventnum, timeout);
    }
    bool AddEvents(int sockfd, uint32_t events) {
        return EpollCtl(sockfd, EPOLL_CTL_ADD, events);
    }
    bool ModEvents(int sockfd, uint32_t events) {
        return EpollCtl(sockfd, EPOLL_CTL_MOD, events);
    }
    bool DelEvents(int sockfd) {
        return EpollCtl(sockfd, EPOLL_CTL_DEL, 0);
    }

    ~Epoller() {
        close(_epfd);
    }

private:
    int _epfd;
};