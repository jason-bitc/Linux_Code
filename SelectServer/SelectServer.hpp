#pragma once

#include <InetAddr.hpp>
#include <iostream>
#include <cstdio>
#include <cstdint>
#include <string>
#include <memory>
#include <sys/select.h>
#include <sys/types.h>
#include "Logger.hpp"
#include "Socket.hpp"

using namespace NS_LOG_MODULE;
using namespace NS_SOCKET_MODULE;

static const int gfdnum = sizeof(fd_set) * 8;
static const int gdefaultfd = -1;

class SelectServer {
public:
    SelectServer(uint16_t port)
        : _port(port), _quit(false), _listensocket(std::make_unique<TcpSocket>()) {
        _listensocket->BuildTcpSocketMethod(port);
        LOG(LogLevel::DEBUG) << "create listensock success, fd : " << _listensocket->Sockfd();

        for(int i = 0; i < gfdnum; i++)
            _rfdset[i] = gdefaultfd;

        // 默认直接把刚开始的fd,直接添加到数组中
        _rfdset[0] = _listensocket->Sockfd();
    }

    void HandlerEvent(fd_set &rds) {
        for(int i = 0; i < gfdnum; i++)
        {
            if(_rfdset[i] == gdefaultfd)
                continue;
            else
            {
                if(FD_ISSET(_rfdset[i], &rds))
                {
                    if(_rfdset[i] == _listensocket->Sockfd())
                    {
                        LOG(LogLevel::INFO) << "listensockfd event ready!";
                        InetAddr clientaddr;
                        int sockfd = _listensocket->Accepter(clientaddr);
                        LOG(LogLevel::INFO)
                            << "get a new link, sockfd is :" << sockfd << clientaddr.ToString();
                        int pos = 0;
                        for(; pos < gfdnum; pos++)
                        {
                            if(_rfdset[pos] == gdefaultfd)
                                break;
                        }
                        if(pos == gfdnum)
                        {
                            LOG(LogLevel::WARNING) << "server is full!";
                            close(sockfd);
                        }
                        else
                            _rfdset[pos] = sockfd;
                    }
                    else
                    {
                        char buffer[1024];
                        ssize_t n = recv(_rfdset[i], buffer, sizeof(buffer) - 1, 0);
                        if(n > 0)
                        {
                            buffer[n] = 0;
                            LOG(LogLevel::DEBUG) << "buffer : " << buffer;
                            std::string echo_string = "echo #";
                            echo_string += buffer;
                            send(_rfdset[i], echo_string.c_str(), echo_string.size(), 0);
                        }
                        else if(n == 0)
                        {
                            LOG(LogLevel::INFO) << "client quit: " << _rfdset[i];
                            close(_rfdset[i]);
                            // 1. 从select移除掉fd
                            _rfdset[i] = gdefaultfd;
                            break;
                        }
                        else if(n < 0)
                        {
                            LOG(LogLevel::WARNING) << "recv error: " << _rfdset[i];
                            close(_rfdset[i]);
                            // 1. 从select移除掉fd
                            _rfdset[i] = gdefaultfd;
                            break;
                        }
                    }
                }
            }
        }
    }
    void Loop() {
        while(!_quit)
        {
            fd_set rfds;
            FD_ZERO(&rfds);
            int maxfd = gdefaultfd;
            for(int i = 0; i < gfdnum; i++)
            {
                if(_rfdset[i] == gdefaultfd)
                    continue;
                FD_SET(_rfdset[i], &rfds);
                if(maxfd < _rfdset[i])
                    maxfd = _rfdset[i];
            }

            int n = select(maxfd + 1, &rfds, nullptr, nullptr, nullptr);
            switch(n)
            {
                case 0:
                    LOG(LogLevel::INFO) << "timeout";
                    break;
                case -1:
                    LOG(LogLevel::WARNING) << "select error";
                    break;
                default:
                    LOG(LogLevel::INFO) << "event ready! n = " << n;
                    HandlerEvent(rfds); // 不仅仅处理新链接，还要处理普通的IO事件！！
                    break;
            }
        }
    }
    ~SelectServer() {}

private:
    uint16_t _port;
    std::unique_ptr<Socket> _listensocket;
    bool _quit;
    int _rfdset[gfdnum];
};