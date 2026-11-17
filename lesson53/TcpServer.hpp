#pragma once

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <functional>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Logger.hpp"
#include "InetAddr.hpp"
#include "Thread.hpp"
#include "ThreadPool.hpp"

using namespace NS_LOG_MODULE;
using namespace NS_THREAD_MODULE;
using namespace NS_THREAD_POOL_MODULE;

using task_t = std::function<void()>;
using handler_t = std::function<std::string(std::string())>;
using _HandlerTips_t = std::function<std::string()>;
enum { SUCCESS = 0, USAGE_ERR, SOCKET_ERR, BIND_ERR, LISTEN_ERR, FORK_ERR };

static const int gbacklog = 16;
static const uint16_t gport = 8888;

class TcpServer {
public:
    TcpServer(uint16_t port = gport) : _port(port) {}
    void InitServer() {
        _listensockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(_listensockfd < 0)
        {
            LOG(LogLevel::FATAL) << "Create ListenSocket Failed! ";
            exit(SOCKET_ERR);
        }
        LOG(LogLevel::DEBUG) << "Create ListenSocket Success! ";
        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = INADDR_ANY;
        local.sin_port = htons(_port);

        int n = bind(_listensockfd, (struct sockaddr *)&local, sizeof(local));
        if(n < 0)
        {
            LOG(LogLevel::FATAL) << "Bind Socket error";
            exit(BIND_ERR);
        }
        LOG(LogLevel::DEBUG) << "Bind Socket Success! ";
        n = listen(_listensockfd, gbacklog);
        if(n < 0)
        {
            LOG(LogLevel::FATAL) << "Listen error";
            exit(LISTEN_ERR);
        }
        LOG(LogLevel::DEBUG) << "Listen Success! ";
    }
    void Register(handler_t handler, _HandlerTips_t HandlerTips) {
        _handler = handler;
        _HandlerTips = HandlerTips;
    }
    int HandlerIO(int sockfd, InetAddr address) {
        char inbuffer[1024] = {0};
        ssize_t n = read(sockfd, inbuffer, sizeof(inbuffer) - 1);
        if(n > 0)
        {
            inbuffer[n] = 0;
            std::string result;
            if(nullptr != _handler)
                result = _handler(inbuffer);
            else
            {
                LOG(LogLevel::WARNING) << "server not init !";
            }
            write(sockfd, result.c_str(), result.size());
        }
        else if(n == 0)
        {
            LOG(LogLevel::INFO) << "client quit adress: " << address.ToString();
        }
        else
        {
            LOG(LogLevel::ERROR) << "client read error";
        }
        return n;
    }
    void ReturnTips(int sockfd, InetAddr address) {
        std::string result = _HandlerTips();
        write(sockfd, result.c_str(), result.size());
    }
    void service(int sockfd, InetAddr address) {
        LOG(LogLevel::DEBUG) << "client info is: " << address.ToString();
        while(true)
        {
            ReturnTips(sockfd, address);
            if(HandlerIO(sockfd, address) <= 0)
                break;
        }
        close(sockfd);
    }
    void Start() {
        while(true)
        {
            struct sockaddr_in clientaddr;
            socklen_t len = sizeof(clientaddr);
            int sockfd = accept(_listensockfd, (struct sockaddr *)&clientaddr, &len);
            if(sockfd < 0)
            {
                LOG(LogLevel::WARNING) << "accept error";
                continue;
            }
            LOG(LogLevel::DEBUG) << "accept success! sockfd: " << sockfd;
            InetAddr clientaddress(clientaddr);
            ThreadPool<task_t>::Instance()->Enqueue([this,sockfd,clientaddress](){
                this->service(sockfd,clientaddress);
            });
        }
    }

private:
    uint16_t _port;
    int _listensockfd;
    handler_t _handler;
    _HandlerTips_t _HandlerTips;
};