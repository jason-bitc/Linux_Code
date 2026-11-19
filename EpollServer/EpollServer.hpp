#include <cstdint>
#include <iostream>
#include <string>
#include <memory>
#include <sys/epoll.h>
#include <sys/types.h>

#include "InetAddr.hpp"
#include "Socket.hpp"
#include "Logger.hpp"

using namespace NS_LOG_MODULE;
using namespace NS_SOCKET_MODULE;

static const int defaultport = 8080;
static const int ign_size = 256;
static const int revs_num = 64;

class EpollServer {
public:
    EpollServer(uint16_t port = defaultport) : _port(port), 
    _quit(false),
    _listensocket(std::make_unique<TcpSocket>())
    {
        _listensocket->BuildTcpSocketMethod(port);
        LOG(LogLevel::INFO) << "create listen socket success, sockfd is : "
                            << _listensocket->Sockfd(); // 3
        _epfd = epoll_create(ign_size);
        if(_epfd < 0)
        {
            LOG(LogLevel::FATAL) << "create epoll error, return : " << _epfd;
            return;
        }
        LOG(LogLevel::INFO) << "create epoll success, epfd is : " << _epfd; // 4
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = _listensocket->Sockfd();
        int n = epoll_ctl(_epfd,EPOLL_CTL_ADD,_listensocket->Sockfd(),&ev);
        if(n == 0)
        {
            LOG(LogLevel::INFO) << "add listen sock to epoll success..."; // 4
        }
        
    }
    void IOService(int sockfd) {
        char inbuffer[1024];
        ssize_t n = recv(sockfd,inbuffer,sizeof(inbuffer)-1,0);
        if(n > 0)
        {
            inbuffer[n] = 0;
            LOG(LogLevel::INFO) << "inbuffer: " << inbuffer;
            std::string echo_string = "echo # ";
            echo_string += inbuffer;

            send(sockfd, echo_string.c_str(), echo_string.size(), 0); // 能直接发送吗？？
        }else if(n == 0)
        {
            LOG(LogLevel::INFO) << "client quit: " << sockfd;
            int n = epoll_ctl(_epfd,EPOLL_CTL_DEL,sockfd,nullptr);
            close(sockfd);
        }else {
            LOG(LogLevel::INFO) << "client quit: " << sockfd;
            int n = epoll_ctl(_epfd, EPOLL_CTL_DEL, sockfd, nullptr);
            close(sockfd);
        }
        
    }
    void Listener() {
        InetAddr clientaddr;
        int newsockfd = _listensocket->Accepter(clientaddr);
        LOG(LogLevel::INFO) << "accept success, new sockfd: " << newsockfd << " "
                            << clientaddr.ToString();
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = newsockfd;
        int  n = epoll_ctl(_epfd,EPOLL_CTL_ADD,newsockfd,&ev);
    }
    void HandleEvents(struct epoll_event revs[], int n) {
        for(int i = 0 ; i < n ; i++)
        {
            int sockfd = revs[i].data.fd;
            uint32_t revents = revs[i].events;
            if(revents & EPOLLIN )
            {
                if(sockfd == _listensocket->Sockfd())
                {
                    Listener();
                }else {
                    IOService(sockfd);
                }
            }

        }
    }
    void Loop()
    {
        int timeout = -1;
        while(!_quit)
        {
            struct epoll_event revs[revs_num];
            int n = epoll_wait(_epfd, revs, revs_num, timeout);
            if(n > 0)
            {
                // 有事件就绪了
                LOG(LogLevel::DEBUG) << "Event Ready!, n = " << n;
                HandleEvents(revs, n);
            }
            else if(n == 0)
            {
                LOG(LogLevel::INFO) << "time out...";
            }
            else
            {
                LOG(LogLevel::ERROR) << "epoll_wait error";
                break;
            }
        }
    }
private:
    uint16_t _port;
    std::unique_ptr<Socket> _listensocket;

    int _epfd;
    bool _quit;
};