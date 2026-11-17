#ifndef __CONNECTION_HPP
#define __CONNECTION_HPP

#include <ctime>
#include <cstdint>
#include <string>
#include <functional>
#include <sys/epoll.h>
#include <unistd.h>
#include "InetAddr.hpp"
class Reactor;

class Connection {
public:
    Connection() : _sockfd(-1), _events(EPOLLIN), _active_time(time(nullptr)) {}
    Connection(int sockfd, uint32_t events)
        : _sockfd(sockfd), _events(events), _active_time(time(nullptr)) {}
    ~Connection() {}
    void Active() {
        _active_time = time(nullptr);
    }
    time_t lastActiveTime() {
        return _active_time;
    }
    virtual int Sender() {}
    virtual int Recver() {}
    virtual int Excepter() {}

    int Sockfd() {
        return _sockfd;
    }
    void Close() {
        close(_sockfd);
    }
    uint32_t Events() {
        return _events;
    }
    void SetEvents(uint32_t events) {
        _events = events;
    }
    void InitAddr(const InetAddr &addr) {
        _clientaddr = addr;
    }
    InetAddr Addr() {
        return _clientaddr;
    }

protected:
    int _sockfd;
    uint32_t _events;
    time_t _active_time;

    std::string _inbuffer;
    std::string _outbuffer;

    InetAddr _clientaddr;

public:
    Reactor *R;
};
#endif