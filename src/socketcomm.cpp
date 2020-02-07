#include "socketcomm.hpp"

cSocketComm::cSocketComm()
{
    sockfd = 0, n = 0;
}


cSocketComm::~cSocketComm()
{

}



bool cSocketComm::initConnection()
{
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cout<<"\n Error : Could not create socket \n";
        return false;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(9001); 

    if(inet_pton(AF_INET, "192.168.42.1", &serv_addr.sin_addr)<=0)
    {
        std::cout<<"\n inet_pton error occured\n";
        return false;
    } 

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cout<<"\n Error : Connect Failed \n";
        return false;
    } 

    return true;
}




std::string cSocketComm::readSocket()
{
    // create the buffer with space for the data
    std::vector<char> buffer(MAX_BUF_LENGTH);
    std::string rcv;   
    //buffer.clear();
    int bytesReceived = 0;
    bytesReceived = recv(sockfd, &buffer[0], buffer.size(), 0);
    // append string from buffer.
    if ( bytesReceived == -1 ) { 
        // error 
    } else {
        rcv.append( buffer.cbegin(), buffer.cend() );
    }

    //std::cout<<rcv<<std::endl;
    //buffer.clear();
    //rcv.clear();
    return rcv;
}
