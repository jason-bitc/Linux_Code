#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <cerrno>
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include "Thread.hpp"
#include "InetAddr.hpp"
#include "UdpServer.hpp"

using namespace NS_THREAD_MODULE;

int sockfd = -1;
std::string server_ip;
uint16_t server_port = 0;
std::string nickname;

static void Usage(const std::string &proc) {
    std::cout << "Usage\n\t";
    std::cout << proc << " server_ip server_port" << std::endl;
}
static void Online(InetAddr &serveraddr) {
    std::cout << "Please Set Your Nick Name: ";
    std::getline(std::cin, nickname);
    std::string online_message = nickname + " online!";
    ssize_t n = sendto(sockfd, online_message.c_str(), online_message.size(), 0,
                       serveraddr.GetNetAddress(), serveraddr.Len());
    if(n < 0)
    {
        std::perror("sendto online message");
    }
}
void RecvMessage() {
    while(true)
    {
        char inbuffer[1024];
        struct sockaddr_in temp;
        socklen_t len = sizeof(temp);
        ssize_t m =
            recvfrom(sockfd, inbuffer, sizeof(inbuffer) - 1, 0, (struct sockaddr *)&temp, &len);
        if(m > 0)
        {
            inbuffer[m] = 0;
            std::cerr << inbuffer << std::endl;
        }
        else if(m < 0 && errno != EINTR)
        {
            std::perror("recvfrom");
            return;
        }
    }
}

void SendMessage() {
    InetAddr serveraddr(server_port, server_ip);
    Online(serveraddr);
    while(true)
    {
        std::string message;
        std::cout << "Please Enter: ";
        if(!std::getline(std::cin, message))
        {
            return;
        }
        message = nickname + ": " + message;
        ssize_t n = sendto(sockfd, message.c_str(), message.size(), 0, serveraddr.GetNetAddress(),
                           serveraddr.Len());
        if(n < 0)
        {
            std::perror("sendto message");
        }
    }
}

int main(int argc, char *argv[]) {
    if(argc != 3)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    server_ip = argv[1];
    server_port = std::stoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);//这里不要忘记创建
    if(sockfd < 0)
    {
        std::perror("socket");
        exit(SOCKET_ERR);
    }
    Thread recver(RecvMessage);
    Thread sender(SendMessage);
    recver.Start();
    sender.Start();
    recver.Join();
    sender.Join();
    return 0;
}
