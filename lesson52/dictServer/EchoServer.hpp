#ifndef __ECHOSERVER_HPP
#define __ECHOSERVER_HPP

#include <iostream>
#include <string>
#include <cstdlib>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <functional>
#include <arpa/inet.h>

#include "Logger.hpp"

using namespace NS_LOG_MODULE;

const static int default_fd = -1;
const static int default_port = 8888;

using callback_t = std::function<std::string(std::string)>;

enum {
    SUCCESS = 0,
    USAGE_ERR,
    SOCKET_ERR,
    BIND_ERR,
};

class UdpServer {
public:
    // UdpServer(const std::string &ip, uint16_t port = default_port)
    UdpServer(callback_t cb, uint16_t port = default_port)
        : _port(port), _sockfd(default_fd), _cb(cb) {}
    ~UdpServer() {
        close(_sockfd);
    }
    void Init() {
        // 第一步: 创建socket, 本质: 打开网卡 --- 系统特性
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if(_sockfd < 0)
        {
            LOG(LogLevel::FATAL) << "create socket error";
            exit(SOCKET_ERR);
        }
        LOG(LogLevel::INFO) << "create socket success, sockfd: " << _sockfd;

        // 第二步: 填充网络信息,
        // 有没有IP和端口信息设置到内核中？？设置到你刚刚打开的网络socket对应的文件内部？
        struct sockaddr_in
            local; // struct sockaddr_in 数据类型，local 用户栈上的！！！，并没有设置到内核
        bzero(&local, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(_port); // h->n
        // local.sin_addr.s_addr = inet_addr(_ip.c_str()); // 1. 字符串ip->4字节IP 2. hton
        local.sin_addr.s_addr = INADDR_ANY; // 最佳实践：任意IP地址bind

        // 第三步：bind socket 信息
        int n = bind(_sockfd, (struct sockaddr *)&local, sizeof(local));
        if(n < 0)
        {
            LOG(LogLevel::FATAL) << "bind socket error";
            exit(BIND_ERR);
        }
        LOG(LogLevel::INFO) << "bind socket success" << ", port: " << _port;
    }
    void Start() {
        // 传递的是字符串,echo server
        char inbuffer[1024];
        while(true)
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            // 1. 用户发来的数据
            // 2. 用户的socket信息
            ssize_t n = recvfrom(_sockfd, inbuffer, sizeof(inbuffer) - 1, 0,
                                 (struct sockaddr *)&peer, &len);
            if(n > 0)
            {
                // ?
                // 用户的IP + Port信息,是recvfrom从网络中获取到的数据，网络序列（大端）->
                // 转换成为主机序列 peer.sin_port; // peer client port peer.sin_addr; // peer client
                // ip
                uint16_t client_port = ntohs(peer.sin_port);
                std::string client_ip = inet_ntoa(peer.sin_addr); // 4字节IP-> ntoh -> 字符串
                std::string client_address =
                    "[" + client_ip + ":" + std::to_string(client_port) + "]# ";

                // 用户发来的数据
                inbuffer[n] = 0;
                // LOG(LogLevel::DEBUG) << client_address << inbuffer;
                // std::string echo_string = "server echo# ";
                // echo_string += inbuffer;
                std::string result = _cb(inbuffer); // 单词

                // h to n
                sendto(_sockfd, result.c_str(), result.size(), 0, (struct sockaddr *)&peer, len);
            }
            else
            {
                LOG(LogLevel::ERROR) << "recvfrom error";
            }
        }
    }

private:
    int _sockfd;
    // std::string _ip; // "192.168.2.2"(字符串风格的点分十进制IP地址, 让人看的) && 4字节IP ???
    uint16_t _port; // 用户设置好的，server port必须是固定的！
    callback_t _cb; // 用回调的方式进行数据加工
};

#endif