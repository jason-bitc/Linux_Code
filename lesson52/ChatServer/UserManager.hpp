#pragma once

#include <vector>
#include "InetAddr.hpp"

class UserManager {
public:
    UserManager() {}
    ~UserManager() {}
    void AddUser(const InetAddr &addr) {
        _users.emplace_back(addr);
    }
    void DelUser(const InetAddr &addr) {
        for(auto iter = _users.begin(); iter != _users.end(); iter++)
        {
            if(*iter == addr)
            {
                _users.erase(iter);
                break;
            }
        }
    }
    bool SearchUser(const InetAddr &addr) {
        for(auto &user : _users)
        {
            if(user == addr)
            {
                return true;
            }
        }
        return false;
    }
    void ModUser(const InetAddr &addr) {
        DelUser(addr);
        AddUser(addr);
    }
    std::vector<InetAddr> &Users() {
        return _users;
    }

private:
    std::vector<InetAddr> _users;
};