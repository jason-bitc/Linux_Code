#include "Logger.hpp"
#include "InetAddr.hpp"
#include "Socket.hpp"
#include "SSL.hpp"
#include <memory>
#include <unistd.h>
#include <signal.h>
#include <functional>

static uint16_t gport = 8080;

using namespace NS_SOCKET_MODULE;

using Handler_t = std::function<std::string(std::string &)>;

class TcpServer
{
public:
    TcpServer(Handler_t handler, uint16_t port = gport)
        : _port(port),
          _listensock(std::make_unique<TcpSocket>()),
          _handler(handler)
    {
        _listensock->BuildTcpSocketMethod(_port); // 使用创建listensockfd的模版方法！
        LOG(LogLevel::INFO) << "create listen socket success: " << _listensock->Sockfd();
    }
    void Loop()
    {
        signal(SIGCHLD, SIG_IGN);
        while (true)
        {
            InetAddr clientaddr;
            auto sockfd = _listensock->Accepter(clientaddr);
            if (!sockfd)
                continue;
            LOG(LogLevel::DEBUG) << "get a new link, socket address: " << clientaddr.ToString() << " sockfd: " << sockfd->Sockfd();

            if (fork() == 0)
            {
                // child
                service(sockfd, clientaddr);
                sockfd->Close();
                exit(0);
            }
        }
    }
    ~TcpServer()
    {
    }

private:
    // 短服务
    void service(std::shared_ptr<Socket> sockfd, InetAddr &clientaddr)
    {
        std::string inbuffer, outbuffer;
        outbuffer.clear();
        // while (1)
        // {
        int n = sockfd->Recv(&inbuffer); // 读到完整的HTTP请求
        if (n <= 0)
        {
            LOG(LogLevel::WARNING) << "recv: client quit, " << clientaddr.ToString();
            return;
        }

        // 解密工作
        // inbuffer = ssl.解密(inbuffer);

        if (_handler)
            outbuffer = _handler(inbuffer);

        // if (outbuffer.empty())
        //     return;

        // 加密工作
        // outbuffer = ssl.加密(outbuffer);

        n = sockfd->Send(outbuffer);
        if (n < 0)
        {
            LOG(LogLevel::WARNING) << "send: client quit, " << clientaddr.ToString();
        }
        // }
    }

private:
    uint16_t _port;
    std::unique_ptr<Socket> _listensock;
    Handler_t _handler;
};