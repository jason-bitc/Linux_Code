#pragma once

#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <cstdio>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>

class Command {
private:
    void LoadCommand() {
        _whitelist.push_back("ls -a -l");
        _whitelist.push_back("ls -l");
        _whitelist.push_back("pwd");
        _whitelist.push_back("whoami");
        _whitelist.push_back("who");
        _whitelist.push_back("ps -al");
        _whitelist.push_back("netstat -nltp");
        _whitelist.push_back("netstat -nutp");
        _whitelist.push_back("netstat -natp");
        _whitelist.push_back("netstat -naup");
    }
    bool IsSafe(const std::string &cmd)
    {
        for(auto &elem : _whitelist)
        {
            if(elem == cmd)
                return true;
        }
        return false;
    }
public:
    Command() {
        LoadCommand();
    }
    ~Command() {}

private:
    std::vector<std::string> _whitelist;
};