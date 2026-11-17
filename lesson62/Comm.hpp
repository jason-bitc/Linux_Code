#ifndef __COMM_HPP
#define __COMM_HPP

#include <iostream>
#include <fcntl.h>
#include "Logger.hpp"


enum EXIT_CODE { SUCCESS, EPOLLER_ERROR, EPOLLER_WAIT_FATAL };

void SetNonBlock(int sockfd) {
    int fl = fcntl(sockfd, F_GETFL);
    if(fl < 0)
    {
        return;
    }
    fcntl(sockfd, F_SETFL, fl | O_NONBLOCK);
}  
#endif