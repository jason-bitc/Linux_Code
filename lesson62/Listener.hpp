#pragma once
#include <cerrno>
#include <iostream>
#include <memory>
#include <sys/epoll.h>
#include "Comm.hpp"
#include "Connection.hpp"
#include "InetAddr.hpp"
#include "Logger.hpp"
#include "Socket.hpp"

using namespace NS_LOG_MODULE;
using namespace NS_SOCKET_MODULE;

const static int defaultport = 8080;

class Listener : public Connection {
public:
    Listener(on_message_t on_message_helper, uint16_t port)
        : _on_message_helper(on_message_helper), _port(port),
          _listensock(std::make_unique<TcpSocket>(_port)) {
        _listensock->BuildTcpSocketMethod(_port);
        _sockfd = _listensock->Sockfd();
        _events = EPOLLIN | EPOLLET;
        Active();
    }
    int Recver() override {
        LOG(LogLevel::DEBUG) << "事件派发到了Listener";
        while(true)
        {
            InetAddr clientaddr;
            int errcode = 0;
            int sockfd = _listensock->Accepter(clientaddr, &errcode);
            if(sockfd < 0)
            {
                if(errcode == EWOULDBLOCK || errcode == EAGAIN)
                {
                    LOG(LogLevel::INFO) << "ccepter all client done";
                    break;
                }
                else if(errcode == EINTR)
                {
                    continue;
                }
                else
                {
                    LOG(LogLevel::WARNING) << "Accepter Error! ";
                }
            }
            
        }
    }

private:
    uint16_t _port;
    std::unique_ptr<Socket> _listensock;
    on_message_t _on_message_helper;
};