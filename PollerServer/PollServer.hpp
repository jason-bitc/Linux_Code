#pragma once

#include <InetAddr.hpp>
#include <iostream>
#include <cstdio>
#include <cstdint>
#include <string>
#include <memory>
#include <poll.h>
#include "Logger.hpp"
#include "Socket.hpp"

using namespace NS_LOG_MODULE;
using namespace NS_SOCKET_MODULE;

static const int gnum = 4096;
static const int gdefaultfd = -1;

class PollServer {
public:
    PollServer(uint16_t port)
        : _port(port), _listensocket(std::make_unique<TcpSocket>()), _quit(false) {
        _listensocket->BuildTcpSocketMethod(port);
        for(int i = 0; i < gnum; i++)
        {
            _pollfds[i].fd = gdefaultfd;
            _pollfds[i].events = 0;
            _pollfds[i].revents = 0;
        }
        _pollfds[0].fd = _listensocket->Sockfd();
        _pollfds[0].events = POLLIN;
    }
    void Accepter()
    {
        LOG(LogLevel::INFO) << "a new connection come! ";
        InetAddr client;
        int sockfd = _listensocket->Accepter(client);
        int pos = 0;
        for(;pos < gnum ; pos++)
        {
            if(_pollfds[pos].fd == gdefaultfd)
                break;
        }
        if(pos == gnum) 
        {
            LOG(LogLevel::WARNING) << "server is full !";
            close(sockfd);
            return;
        }
        _pollfds[pos].fd = sockfd;
        _pollfds->events = POLLIN;
    }
    void Recver(int i){
        char buffer[1024];
        int n = recv(_pollfds[i].fd,buffer,sizeof(buffer)-1,0);
        if(n > 0)
        {
            buffer[n] = 0;
            std::string echo_string = "echo# ";
            echo_string += buffer;
            send(_pollfds[i].fd,echo_string.c_str(),echo_string.size(),0);
        }else if(n == 0)
        {
            LOG(LogLevel::ERROR) << "client  exit";
            close(_pollfds[i].fd);
            _pollfds[i].fd = gdefaultfd;
            _pollfds[i].events = _pollfds[i].revents = 0;
        }else {
            LOG(LogLevel::ERROR) << "recv error";
            close(_pollfds[i].fd);
            _pollfds[i].fd = gdefaultfd;
            _pollfds[i].events = _pollfds[i].revents = 0;
        }
    }
    void Dispatcher(struct pollfd pollfds[]) {
        for(int i = 0 ; i < gnum ; i++)
        {
            if(pollfds[i].fd == gdefaultfd)
                continue;
            if(pollfds[i].revents & POLLIN)
            {
                if(pollfds[i].fd == _listensocket->Sockfd())
                {
                    Accepter();
                }else {
                    Recver(i);
                }
            }
        }
    }

    void Loop()
    {
        while(!_quit)
        {
            int n = poll(_pollfds,sizeof(_pollfds)/sizeof(_pollfds[0]),-1);//
            switch(n)
            {
                case -1:
                    LOG(LogLevel::ERROR) << "poll error";
                    break;
                case 0:
                    LOG(LogLevel::ERROR) << "Time out!";
                default:
                    Dispatcher();
            }
        }
    }
private:
    uint16_t _port;
    std::unique_ptr<Socket> _listensocket;
    bool _quit;
    struct pollfd _pollfds[gnum];
};