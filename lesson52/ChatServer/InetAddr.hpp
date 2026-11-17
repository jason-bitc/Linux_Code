#pragma once
#include <string>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class InetAddr {
public:
    InetAddr(const struct sockaddr_in &address) : _address(address), _len(sizeof(address)) {
        _ip = inet_ntoa(_address.sin_addr);
        _port = ntohs(address.sin_port);
    }
    InetAddr(uint16_t port, const std::string &ip = "0.0.0.0") : _ip(ip), _port(port) {
        bzero(&_address, sizeof(_address));
        _address.sin_family = AF_INET;
        _address.sin_addr.s_addr = inet_addr(_ip.c_str());
        _address.sin_port = htons(_port);//这里不能用_address.sin.port;
        _len = sizeof(_address);
    }
    InetAddr(){}
    struct sockaddr *GetNetAddress() {
        return (sockaddr *)&_address;
    }
    socklen_t Len() {
        return _len;
    }
    bool operator==(const InetAddr &addr) {
        return (this->_ip == addr._ip) && (this->_port == addr._port);
    }
    std::string Tostring() {
        return "[" + _ip + ":" + std::to_string(_port) + "]";
    }
    ~InetAddr() {}

private:
    std::string _ip;
    uint16_t _port;
    socklen_t _len;
    struct sockaddr_in _address;
};
