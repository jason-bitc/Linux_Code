#include <iostream>
#include <string>
#include <memory>
#include "InetAddr.hpp"
#include "Socket.hpp"
#include "Protocol.hpp"

using namespace NS_SOCKET_MODULE;

static void Usage(const std::string &proc)
{
    std::cout << "Usage:\n\t" << proc << " server_ip server_port" << std::endl;
}

static void HanlderResponse(Response &resp)
{
    std::cout << "result: " << resp._result << "[" << resp._code << "]" << std::endl;
}

// ./netcal_client 目标IP 目标主机端口
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }
    std::string server_ip = argv[1];
    uint16_t server_port = std::stoi(argv[2]);

    std::unique_ptr<Socket> socket = std::make_unique<TcpSocket>();
    socket->BuildTcpClientSockMethod();

    InetAddr serveraddress(server_port, server_ip);
    bool n = socket->Connect(serveraddress);
    if (!n)
    {
        std::cerr << "connect error: " << serveraddress.ToString() << std::endl;
        exit(2);
    }
    Protocol procotol(HanlderResponse);
    std::string inbuffer;
    while (true)
    {
        int cnt = 3;
        std::string outbuffer;
        while (cnt--)
        {
            // 0. 获取数据
            int x, y;
            char oper;
            std::cout << "Enter Your x: ";
            std::cin >> x;
            std::cout << "Enter Your y: ";
            std::cin >> y;
            std::cout << "Enter Your oper: ";
            std::cin >> oper;

            // 1. 定义请求对象
            Request req(x, y, oper);

            // 2. 序列化
            std::string req_json;
            req.Serialize(&req_json);

            // 3. 封装报头
            std::string send_req_string = procotol.Packet(req_json);
            outbuffer += send_req_string;
        }
        std::cout << "\n" << outbuffer << std::endl;

        // 4. 发送
        socket->Send(outbuffer);

        // 5. 接收
        socket->Recv(&inbuffer);

        // 6. 解析
        procotol.ParseResponse(inbuffer);
    }

    // Protocol procotol(HanlderResponse);
    // std::string inbuffer;
    // while (true)
    // {
    //     // 0. 获取数据
    //     int x, y;
    //     char oper;
    //     std::cout << "Enter Your x: ";
    //     std::cin >> x;
    //     std::cout << "Enter Your y: ";
    //     std::cin >> y;
    //     std::cout << "Enter Your oper: ";
    //     std::cin >> oper;

    //     // 1. 定义请求对象
    //     Request req(x, y, oper);

    //     // 2. 序列化
    //     std::string req_json;
    //     req.Serialize(&req_json);

    //     // 3. 封装报头
    //     std::string send_req_string = procotol.Packet(req_json);

    //     // 4. 发送
    //     socket->Send(send_req_string);

    //     // 5. 接收
    //     socket->Recv(&inbuffer);

    //     // 6. 解析
    //     procotol.ParseResponse(inbuffer);
    // }

    socket->Close();
    // sock
    return 0;
}