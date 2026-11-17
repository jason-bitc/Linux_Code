#include "InetAddr.hpp"
#include "Logger.hpp"
#include "UdpServer.hpp"
#include "UserManager.hpp"
#include "ThreadPool.hpp"
#include "Route.hpp"
#include <cstdint>
#include <functional>
#include <memory>
#include <ratio>

static void Usage(const std::string &process) {
    std::cerr << "Usage:\n\t";
    std::cerr << process << " local_port" << std::endl;
}
using namespace NS_LOG_MODULE;
using namespace NS_THREAD_MODULE;
using namespace NS_THREAD_POOL_MODULE;

using task_t = std::function<void()>;

int main(int argc, char *argv[]) {
    if(argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    ENABLE_CONSOLE_LOG_STRATEGY();
    uint16_t server_port = std::stoi(argv[1]);

    auto thread_pool = ThreadPool<task_t>::Instance();

    Route r;

    UdpServer usvr(server_port);
    usvr.Init();

    usvr.RegisterService([&r](const InetAddr & addr){
        r.CheckUser(addr);
    },[&r,thread_pool](int sockfd,std::string &msg){
        auto t = std::bind(&Route::BroadCast,&r,sockfd,msg);//这里的&r传路由对象是为什么
        thread_pool->Enqueue(t);
    });
    usvr.Start();
    return 0;
}
