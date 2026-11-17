#include "TcpServer.hpp"
#include "Protocol.hpp"
#include "Calculator.hpp"
#include <memory>


static void Usage(const std::string &proc)
{
    std::cout << "Usage:\n\t" << proc << " port" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(1);
    }
    uint16_t port = std::stoi(argv[1]);
    // 1. 定义计算机
    std::unique_ptr<Calculator> cal = std::make_unique<Calculator>();

    // 2. 定义协议对象
    std::unique_ptr<Protocol> protocol = std::make_unique<Protocol>(
        [&cal](Request &req)->Response{
            return cal->Execute(req);
        }
    );

    // 3. 定义网络对象
    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(
        [&protocol](std::string &inbuffer)->std::string{
            return protocol->ParseRequest(inbuffer);
        }, port
    );
    
    // 4. 启动
    tsvr->Loop();

    return 0;
}