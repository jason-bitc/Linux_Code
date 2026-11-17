#include <iostream>
#include <string>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "EchoTcpServer.hpp"
#include "InetAddr.hpp"
#include "Logger.hpp"

static void Usage(const std::string &name) {
    std::cerr << "Usage:\n\t";
    std::cerr << name << " server_ip server_port" << std::endl;
}
int main(int argc, char *argv[]) {
    if(argc != 3)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    std::string server_ip = argv[1];
    uint16_t server_port = std::stoi(argv[2]);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        LOG(LogLevel::FATAL) << "Create Socket Failed!";
        exit(SOCKET_ERR);
    }
    InetAddr serveradress(server_port, server_ip);
    int n = connect(sockfd, (struct sockaddr *)serveradress.GetNetAddress(), serveradress.Len());
    if(n < 0)
    {
        std::cerr << "Connect to " << serveradress.ToString() << " failed! " << std::endl;
        exit(3);
    }
    std::cerr << "Connect to " << serveradress.ToString() << " Success! " << std::endl;
    while(true)
    {
        std::string line;
        std::cout << "Please Enter: ";
        std::getline(std::cin, line);

        write(sockfd, line.c_str(), line.size());

        char inbuffer[1024];
        ssize_t n = read(sockfd, inbuffer, sizeof(inbuffer) - 1);
        if(n > 0)
        {
            inbuffer[n] = 0;
            std::cout << inbuffer << std::endl;
        }
        else if(n == 0)
        {
            std::cout << "read enf of file!" << std::endl;
            break;
        }
        else
        {
            std::cerr << "read error!" << std::endl;
            break;
        }
    }
}