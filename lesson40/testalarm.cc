#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

long long cnt = 0;
void handler(int signo) {
    std::cout << "receive a signal: signal num: " << signo << "  cnt:" << cnt << std::endl;
    exit(4);
}
int main() {
    // signal(14, handler);
    alarm(1);
    while(true)
    {
        // std::cout << "testsig... " << cnt << std::endl;
        cnt++;
    }
    return 0;
}