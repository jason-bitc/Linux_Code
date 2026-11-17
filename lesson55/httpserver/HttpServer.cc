#include "TcpServer.hpp"
#include "Http.hpp"
#include "Daemon.hpp"
#include <memory>


static void Usage(const std::string &proc)
{
    std::cout << "Usage:\n\t" << proc << " port" << std::endl;
}

// 使用HTTP 实现restful风格的服务接口
// GET /Login HTTP/1.1
// GET /exec HTTP/1.1
// 
// ls -a -l
// 可不可以是另一个http的client？
void Login(HttpRequest&req, HttpResponse &resp)
{
    std::cout << "\nLogin functon been called!" << std::endl;
    req.DebugPrint();

    std::string data = req.Text();
    std::cout << "data is: " << data << std::endl; //  name=aaaaa&passwd=bbbbb
    // 根据分隔符 name  aaaaa     passwd bbb

    // 访问数据库， 查找数据库

    resp.SetCode(200);
    resp.AddHeader("Content-Type", MiniType::Suffix2MimeType(".txt"));
    resp.SetBody("Login success!");
    resp.AddHeader("Content-Length", std::to_string(resp.BodySize()));
}

void Connect(HttpRequest&req, HttpResponse &resp)
{
    // 我们的http是浏览器或者其他客户端的服务端
    // 但是我们的服务端，可不可以是其他server的客户端！
}
// POST "/api/log" HTTP/1.1
// 
// 代码信息
// 你的代码
// void sort()
// 服务器提供
// int main(){
// sort(arr1), printf(arr1)
// sort(arr2), printf(arr2)
// sort(arr3), printf(arr3)
// sort(arr4), printf(arr4)
// }
// 结果
// 12345
// 6789
void OJ(HttpRequest&req, HttpResponse &resp)
{
    //     pipe();

    // if(fork() == 0)
    // {
    //     dup();
    //     execl("/usr/bin/gcc", ....);
    // }
    // 从管道中，读取python处理完成的结果！
}
void Register(HttpRequest&req, HttpResponse &resp)
{

}

void GetProductList(HttpRequest&req, HttpResponse &resp)
{

}

void Search(HttpRequest&req, HttpResponse &resp)
{

}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(1);
    }
    // ENABLE_FILE_LOG_STRATEGY();
    ENABLE_CONSOLE_LOG_STRATEGY();
    uint16_t port = std::stoi(argv[1]);

    // Daemon(1, 1); 
    // daemon(0, 0);
    // LOG -> 文件版日志
    // printf && cout -> /dev/null

    // 2. 定义HTTP协议
    std::unique_ptr<HttpProtocol> protocol = std::make_unique<HttpProtocol>();
    // GET /yuyinshibie HTTP/1.1
    //
    // {xxxxx}
    protocol->RegisterService("/Login", Login);
    protocol->RegisterService("/Register", Register);
    protocol->RegisterService("/Search", Search);
    protocol->RegisterService("/api/getprocutlist", GetProductList);


    // 3. 定义网络对象
    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(
        [&protocol](std::string &inbuffer)->std::string{
            return protocol->HandlerHttpRequest(inbuffer);
        }, port
    );
    
    // 4. 启动
    tsvr->Loop();

    return 0;
}