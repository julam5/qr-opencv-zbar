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
#include <fcntl.h>

const unsigned int MAX_BUF_LENGTH = 141;

class cSocketComm
{
    private:
        int sockfd, n;
        struct sockaddr_in serv_addr;
        std::string ip_address;
        int port;

    public:
        cSocketComm(std::string ip_address_, int port_)
            : ip_address(ip_address_), port(port_)
        {
            sockfd = 0, n = 0;
        }

        ~cSocketComm()
        {
            close(sockfd);
        }

        bool initConnection();
        bool resetConnection();

        void readSocket(std::string &rcv_, bool& readStatus);
    
};

#endif //SOCKETCOMM_HPP_