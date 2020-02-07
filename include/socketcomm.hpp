#ifndef SOCKETCOMM_HPP_
#define SOCKETCOMM_HPP_

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <sstream>
#include <iostream>
#include <vector>

const unsigned int MAX_BUF_LENGTH = 141;

class cSocketComm
{
    private:
        int sockfd, n;
        struct sockaddr_in serv_addr;

    public:
        cSocketComm();
        ~cSocketComm();

        bool initConnection();

        std::string readSocket();
    
};

#endif //SOCKETCOMM_HPP_