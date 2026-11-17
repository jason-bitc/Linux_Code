#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <cstdlib>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "./Logger.hpp"

using namespace NS_LOG_MODULE;

const static int default_fd = -1;
const static int default_port = 8888;

enum { SUCCESS = 0, USAGE_ERR, SOCKET_ERR, BIND_ERR };

class UdpServer {
public:
    UdpServer(uint16_t port = default_port) : _port(port), _sockfd(default_fd) {}
    void Init() {
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if(_sockfd < 0)
        {
            LOG(LogLevel::FATAL) << "create socket failed! ";
            exit(SOCKET_ERR);
        }
        LOG(LogLevel::INFO) << "create socket success ! socketfd: " << _sockfd;
        struct sockaddr_in local;
        bzero(&local, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        local.sin_addr.s_addr = INADDR_ANY;

        int n = bind(_sockfd, (struct sockaddr *)&local, sizeof(local));
        if(n < 0)
        {
            LOG(LogLevel::FATAL) << "bind socket error! ";
            exit(BIND_ERR);
        }
        LOG(LogLevel::INFO) << "bind socket succcess !" << " port: " << _port;
    }
    void Start() {
        char inbuffer[4096];
        while(true)
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            ssize_t n = recvfrom(_sockfd, inbuffer, sizeof(inbuffer) - 1, 0,
                                 (struct sockaddr *)&peer, &len);
            if(n < 0)
            {
                LOG(LogLevel::ERROR) << "recvfrom ERROR";
                continue;
            }

            uint16_t client_port = ntohs(peer.sin_port);
            std::string client_ip = inet_ntoa(peer.sin_addr);
            std::string client_address = "[" + client_ip + ":" + std::to_string(client_port) + "]#";
            inbuffer[n] = '\0';
            LOG(LogLevel::INFO) << client_address << inbuffer;
            std::string echo_string = "server echo#";
            echo_string += inbuffer;
            sendto(_sockfd, echo_string.c_str(), echo_string.size(), 0, (struct sockaddr *)&peer,
                   len);
        }
    }
    ~UdpServer() {
        close(_sockfd);
    }

private:
    int _sockfd;
    uint16_t _port;
};
