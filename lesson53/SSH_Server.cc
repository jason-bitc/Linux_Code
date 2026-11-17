#include "Command.hpp"
#include "TcpServer.hpp"
#include <memory>

static void Usage(const std::string &process)
{
    std::cerr << "Usage:\n\t";
    std::cerr << process << " local_port" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    ENABLE_CONSOLE_LOG_STRATEGY();
    uint16_t server_port = std::stoi(argv[1]);
    // 命令处理的类对象
    std::unique_ptr<Command> command = std::make_unique<Command>();
    // 网络通信的类
    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(server_port);

    tsvr->Register(
        [&command](std::string cmd) -> std::string
        {
            return command->Excute(cmd);
        },
        [&command]() -> std::string
        {
            return command->GetCommandString();
        }
    );

    tsvr->InitServer();
    tsvr->Start();

    return 0;
}