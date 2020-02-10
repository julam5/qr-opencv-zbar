#include "socketcomm.hpp"


bool cSocketComm::initConnection()
{
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cout<<"Error : Could not create socket"<<std::endl;
        return false;
    }

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port); 

    if(inet_pton(AF_INET, ip_address.c_str(), &serv_addr.sin_addr)<=0)
    {
        std::cout<<"inet_pton error occured"<<std::endl;
        return false;
    } 

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cout<<"Error : Connect Failed"<<std::endl;
        return false;
    } 

    sleep(3);

    return true;
}


bool cSocketComm::resetConnection()
{
    close(sockfd);

    if(this->initConnection())
    {
        std::cout<<"\n Error : Reconnect Failed"<<std::endl;
        return false;
    } 

    return true;
}



void cSocketComm::readSocket(std::string &rcv_, bool& readStatus)
{
    // create the buffer with space for the data
    std::vector<char> buffer(MAX_BUF_LENGTH);
    std::string rcv;   

    int bytesReceived = 0;
    bytesReceived = recv(sockfd, &buffer[0], buffer.size(), 0);
    // append string from buffer.
    if ( bytesReceived == -1 ) { 
        readStatus = false;
        rcv_ = "Cannot read data from port!";
    } else {
        rcv.append( buffer.cbegin(), buffer.cend() );
        readStatus = true;
        rcv_ = rcv;
    }
}
