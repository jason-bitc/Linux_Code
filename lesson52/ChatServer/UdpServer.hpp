#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include "InetAddr.hpp"
#include "Logger.hpp"

using namespace NS_LOG_MODULE;

const static int default_fd = -1;
const static uint16_t default_port = 8888;

using handler_addr_t = std::function<void(const InetAddr &)>;
using handler_msg_t = std::function<void(int sockfd, std::string &msg)>;

enum {
    SUCCESS,
    USAGE_ERR,
    SOCKET_ERR,
    BIND_ERR,
};
class UdpServer {
public:
    UdpServer(uint16_t port = default_port) : _port(port), _sockfd(default_fd) {}
    void Init() {
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if(_sockfd < 0)
        {
            LOG(LogLevel::FATAL) << "create socket failed: " << std::strerror(errno);
            exit(SOCKET_ERR);
        }
        LOG(LogLevel::INFO) << "create socket success! socketfd: " << _sockfd;
        InetAddr local(_port);
        int n = bind(_sockfd, local.GetNetAddress(), local.Len());
        if(n < 0)
        {
            LOG(LogLevel::FATAL) << "Bind Socket Failed: " << std::strerror(errno);
            exit(BIND_ERR);
        }
        LOG(LogLevel::INFO) << "Bind Socket Success ! ";
    }
    void RegisterService(handler_addr_t handler_addr, handler_msg_t handler_msg) {
        _handler_addr = handler_addr;
        _handler_msg = handler_msg;
    }
    void Start() {
        char inbuffer[1024];
        while(true)
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            ssize_t n = recvfrom(_sockfd, inbuffer, sizeof(inbuffer) - 1, 0,
                                 (struct sockaddr *)&peer, &len);
            if(n > 0)
            {
                inbuffer[n] = 0;
                InetAddr ClientAddrss(peer);
                std::string tips = ClientAddrss.Tostring();
                std::string message = tips + inbuffer;
                LOG(LogLevel::INFO) << message;
                _handler_addr(ClientAddrss);

                _handler_msg(_sockfd, message); //
            }
            else if(n < 0 && errno != EINTR)
            {
                LOG(LogLevel::ERROR) << "Recvfrom Error: " << std::strerror(errno);
            }
        }
    }
    ~UdpServer() {
        close(_sockfd);
    }

private:
    uint16_t _port;
    int _sockfd;
    handler_addr_t _handler_addr;
    handler_msg_t _handler_msg;
};
