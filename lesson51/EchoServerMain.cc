#include "EchoServer.hpp"
#include "Logger.hpp"
#include <memory>

static void Usage(const std::string &process) {
    std::cerr << "Usage:\n\t";
    std::cerr << process << " local_port" << std::endl;
}

int main(int argc, char *argv[]) {
    if(argc != 2)
    {
        Usage(argv[0]);
        exit(1);
    }
    ENABLE_CONSOLE_LOG_STRATEGY();
    uint16_t port = std::stoi(argv[1]);
    UdpServer server(port);
    server.Init();
    server.Start();
    return 0;
}