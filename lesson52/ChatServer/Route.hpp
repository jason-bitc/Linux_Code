#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <sys/socket.h>
#include "InetAddr.hpp"
#include "Mutex.hpp"
#include "UserManager.hpp"

class Route {
public:
    Route() : _uma(std::make_unique<UserManager>()) {}
    void CheckUser(const InetAddr &addr)
    {
        LockGuard lockguard(_mutex);
        _uma->AddUser(addr);
    }
    void OfflineUser(const InetAddr& addr)
    {
        LockGuard lockguard(_mutex);
        _uma->DelUser(addr);
    }
    void BroadCast(int sockfd,std::string message)
    {
        LockGuard lockguard(_mutex);
        auto &users = _uma->Users();
        for(auto & user : users)
        {
             sendto(sockfd,message.c_str(),message.size(),0,(struct sockaddr*)user.GetNetAddress(),user.Len());
        }
    }
    ~Route(){}
private:
    std::unique_ptr<UserManager> _uma;
    Mutex _mutex;
};