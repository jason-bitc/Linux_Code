#include "BlockQueue.hpp"

void* ConsumerRoutine(void* args){}
void* ProductorRoutine(void* args){}
int main(){
    BlockQueue<int> *bq = new BlockQueue<int>();
    pthread_t c,p;
    pthread_create(&c,nullptr,ConsumerRoutine,nullptr);
    pthread_create(&p,nullptr,ProductorRoutine,nullptr);

    pthread_join(c,nullptr);
    pthread_join(p,nullptr);
    return 0;
}