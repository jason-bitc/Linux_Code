#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
void handler(int signo) {
    std::cout << "receive a signal: signal num: " << signo << std::endl;
    exit(10);
}
int main() {
    signal(2, handler);
    while(true)
    {
        std::cout << "testsig..." << " process pid: " << getpid() << std::endl;
        sleep(1);
    }
    return 0;
}