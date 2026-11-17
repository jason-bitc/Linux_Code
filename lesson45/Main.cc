#include "Thread.hpp"
#include <pthread.h>
#include <unistd.h>
#include <vector>
void loop() {
    char name[64];
    int count = 1;
    pthread_getname_np(pthread_self(), name, sizeof(name));
    while(true)
    {
        std::cout << "new thread running: " << name << " count: " << count++ << std::endl;
        sleep(1);
    }
}
int main() {
    // ThreadModule::Thread t(loop);
    // t.Start();
    // sleep(1);
    // t.Detach();
    // sleep(10);
    // t.Stop();
    // t.Join();
    // t.PrintInfo();
    std::vector<ThreadModule::Thread> v;
    return 0;
}