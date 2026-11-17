#include "Reactor.hpp"
#include "Connection.hpp"
#include "Logger.hpp"
#include <memory>

int main(int argc, char *argv[]) {
    if(argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " port" << std::endl;
        return 1;
    }
    ENABLE_CONSOLE_LOG_STRATEGY();

    uint16_t port = std::stoi(argv[1]);

    std::shared_ptr<Reactor> R = std::make_shared<Reactor>();

    return 0;
}