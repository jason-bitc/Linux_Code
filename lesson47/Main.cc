#include "BlockQueue.hpp"
#include "Thread.hpp"
#include "Task.hpp"
#include <memory>
#include <unistd.h>
#include <ctime>
#include <cstdlib>


int num = 1;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int GetNumber()
{
    pthread_mutex_lock(&lock);
    int number = num++;
    pthread_mutex_unlock(&lock);

    return number;
}

using namespace ThreadModule;

int main()
{
    srand(time(nullptr) ^ getpid());
    std::unique_ptr<BlockQueue<task_t>> bq = std::make_unique<BlockQueue<task_t>>();

    Thread consumer([&bq](){
        while (true)
        {
            sleep(1);
            task_t t;
            // 1. 取数据 - 消费
            bq->Pop(&t);
            // 2. 处理数据 --- 重要！
            // t.Execute();
            t();
            // std::cout  << " 消费: " << t.getResult() << std::endl;
        }
    });

    Thread productor([&bq]() {        
        while (true)
        {
            // 1. 获取数据或者任务
            int datax = rand() % 10 + 1;
            usleep(rand()%1357);
            int datay = rand() % 10 + 1;

            // 2. 生产
            // Task t(datax, datay);
            bq->Enqueue(Print);
            // std::cout << " 生产: " << t.Question() << std::endl;
        }
    });

    consumer.Start();
    productor.Start();

    consumer.Join();
    productor.Join();

    // pthread_t c[3], p[2];
    // pthread_create(c, nullptr, ConsumerRoutine, bq);
    // pthread_create(c + 1, nullptr, ConsumerRoutine, bq);
    // pthread_create(c + 2, nullptr, ConsumerRoutine, bq);
    // pthread_create(p, nullptr, ProductorRouine, bq);
    // pthread_create(p + 1, nullptr, ProductorRouine, bq);

    // pthread_join(c[0], nullptr);
    // pthread_join(c[1], nullptr);
    // pthread_join(c[2], nullptr);
    // pthread_join(p[0], nullptr);
    // pthread_join(p[1], nullptr);

    return 0;
}