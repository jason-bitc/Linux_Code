#pragma once

#include "InetAddr.hpp"
#include <cerrno>
#include <fstream>
#include <ios>
#include <iostream>
#include <cstdio>
#include <cstdint>
#include <string>
#include <memory>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>
#include "Logger.hpp"
#include "Socket.hpp"

using namespace NS_LOG_MODULE;
using namespace NS_SOCKET_MODULE;

static const int gfdnum = sizeof(fd_set) * 8;
static const int gdefaultfd = -1;
static const std::string defaultconfigpath = "./dict.txt";
static const std::string defaultWordSep = ":";
static const std::string echo_string = "Please Enter Your Chinese word: ";

using std::string;
class Dict {
public:
    static const std::unordered_map<std::string, std::string> &dicts() {
        static const auto dict = [] {
            std::unordered_map<std::string, std::string> d;
            std::fstream in(defaultconfigpath, std::ios::in);
            if(!in)
            {
                LOG(LogLevel::FATAL) << "Load Dict Failed!";
                exit(2);
            }
            std::string line;
            while(std::getline(in, line))
            {
                auto pos = line.find(defaultWordSep);
                if(pos == std::string::npos)
                {
                    LOG(LogLevel::WARNING) << "dict format error: " << line;
                    continue;
                }
                d.emplace(line.substr(0, pos), line.substr(pos + defaultWordSep.size()));
            }
            return d;
        }();
        return dict;
    }
};

class TranslateServer {
public:
    TranslateServer(uint16_t port = 8080)
        : _port(port), _quit(false), _listensock(std::make_unique<TcpSocket>()), _dicts() {
        _listensock->BuildTcpSocketMethod(_port);
        for(int i = 0; i < gfdnum; i++)
            _rfdset[i] = gdefaultfd;
        _rfdset[0] = _listensock->Sockfd();
    }
    void HandleEvents(fd_set &rfds) {
        for(int i = 0; i < gfdnum; i++)
        {
            if(_rfdset[i] == gdefaultfd)
                continue;
            if(FD_ISSET(_rfdset[i], &rfds))
            {
                if(_rfdset[i] == _listensock->Sockfd())
                {
                    InetAddr clientaddr;
                    int sockfd = _listensock->Accepter(clientaddr);
                    if(sockfd < 0)
                        continue;
                    if(sockfd >= FD_SETSIZE)
                    {
                        LOG(LogLevel::WARNING) << "sockfd exceeds select limit";
                        close(sockfd);
                        continue;
                    }
                    LOG(LogLevel::INFO) << "a new connection come! sockfd is : " << sockfd;
                    int pos = 0;
                    for(; pos < gfdnum; pos++)
                    {
                        if(_rfdset[pos] == gdefaultfd)
                            break;
                    }
                    if(pos == gfdnum)
                    {
                        LOG(LogLevel::WARNING) << "server is full !";
                        close(sockfd);
                        continue;
                    }
                    _rfdset[pos] = sockfd;
                    _inbuffers[pos].clear();
                    send(sockfd, echo_string.c_str(), echo_string.size(), 0);
                }
                else
                {
                    char buffer[1024];
                    ssize_t n = recv(_rfdset[i], buffer, sizeof(buffer), 0);
                    if(n > 0)
                    {
                        auto &input = _inbuffers[i];
                        input.append(buffer, static_cast<std::size_t>(n));//这里的n必须加上?

                        // 协议：一行一个单词。处理所有完整行，保留末尾半包。
                        std::string::size_type pos;
                        while((pos = input.find('\n')) != std::string::npos)
                        {
                            if(pos > kMaxLineLength)
                            {
                                LOG(LogLevel::WARNING) << "request line too long";
                                CloseClient(i);
                                break;
                            }
                            std::string word = input.substr(0, pos);
                            input.erase(0, pos + 1);
                            // 兼容客户端使用 CRLF 换行。
                            if(!word.empty() && word.back() == '\r')
                                word.pop_back();
                            if(word.empty())
                                continue;

                            LOG(LogLevel::DEBUG) << "word is : " << word;
                            const auto &dict = _dicts.dicts();
                            auto it = dict.find(word);
                            std::string response = it == dict.end()
                                ? "未找到该单词" : it->second;
                            // 文本统一使用 UTF-8；CRLF 让终端换行并回到行首。
                            response += "\r\n";
                            response += echo_string;
                            send(_rfdset[i], response.c_str(), response.size(), 0);
                        }
                        // 限制未完成行，避免一直不发换行导致缓存无限增长。
                        if (input.size() > kMaxLineLength)
                        {
                            LOG(LogLevel::WARNING) << "request line too long";
                            CloseClient(i);
                        }
                    }
                    else if(n == 0)
                    {
                        LOG(LogLevel::WARNING) << "client exit ";
                        CloseClient(i);
                        continue;
                    }
                    else
                    {
                        if(errno == EINTR )
                            continue;
                        LOG(LogLevel::WARNING) << "Recv error! ";
                        CloseClient(i);
                        continue;
                    }
                }
            }
        }
    }
    void Loop() {
        while(!_quit)
        {
            fd_set rfds;
            FD_ZERO(&rfds);
            int max_rfd = -1;
            for(int i = 0; i < gfdnum; i++)
            {
                if(_rfdset[i] == gdefaultfd)
                    continue;
                FD_SET(_rfdset[i], &rfds);
                if(max_rfd < _rfdset[i])
                    max_rfd = _rfdset[i];
            }
            int n = select(max_rfd + 1, &rfds, nullptr, nullptr, nullptr); // 最后一个参数了解一下
            switch(n)
            {
                case 0:
                    LOG(LogLevel::INFO) << "timeout";
                    break;
                case -1:
                    LOG(LogLevel::WARNING) << "select error";
                    break;
                default:
                    LOG(LogLevel::INFO) << "event ready! n = " << n;
                    HandleEvents(rfds); // 不仅仅处理新链接，还要处理普通的IO事件！！
                    break;
            }
        }
    }
    ~TranslateServer() {}

private:
    void CloseClient(int i)
    {
        _inbuffers[i].clear();
        close(_rfdset[i]);
        _rfdset[i] = gdefaultfd;
    }

    // 单行最多 4096 字节（不含 LF，含可选的 CR）。
    static constexpr std::size_t kMaxLineLength = 4096;
    uint16_t _port;
    bool _quit;
    std::unique_ptr<Socket> _listensock;
    int _rfdset[gfdnum];
    std::string _inbuffers[gfdnum]; // 与连接槽位对应，跨 recv 保存半包
    Dict _dicts;
};
