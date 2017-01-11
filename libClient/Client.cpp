#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include "libmessage/MessageBuffer"
#include "libmessage/Message"

#define LOG_DEBUG std::cout << __FILE__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|DEBUG|"
#define LOG_ERROR std::cout << __FILE__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|ERROR|"

int Connect()
{
    int iSockFd = ::socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == iSockFd)
	{
		LOG_ERROR << "socket error|msg:" << ::strerror(errno) << std::endl;
		return (-1);
	}

	struct sockaddr_in stSvrAddr;
   	stSvrAddr.sin_family = AF_INET;
   	stSvrAddr.sin_port = htons(8889);
   	stSvrAddr.sin_addr.s_addr = ::inet_addr("127.0.0.1");
   	if(::connect(iSockFd, (struct sockaddr*)&stSvrAddr, sizeof(stSvrAddr)) == -1)
   	{
   		LOG_ERROR << "connect to server error|msg:" << ::strerror(errno) << std::endl;
                  
   		return (-1);
   	} 

   	return (iSockFd);
}

int main(int argc, char** argv)
{
    int iSockFd = Connect();
    if(iSockFd < 0)
    {
        return (0);
    }

    char szBuffer[1024];
    std::string strMsg;
   
    while(1)
    { 
        Message msg;
        std::cout << "please enter message!" << std::endl; 
        std::cin >> strMsg;
        if(strMsg.empty() == false)
        {
            ::write(iSockFd, strMsg.c_str(), strMsg.size());
            ssize_t iReadLen = ::read(iSockFd, szBuffer, sizeof(szBuffer) - 1);
            if(iReadLen < 0)
            {
                LOG_ERROR << "read from server error|msg:" << ::strerror(errno) << std::endl;
                break;
            }
            else if(0 == iReadLen)
            {
                LOG_ERROR << "server request to close server!" << std::endl;
                break;
            }
            else
            {
                szBuffer[iReadLen] = '\0';
                LOG_DEBUG << "read message from server|msg:" << szBuffer << std::endl;
            }
        }
        else
        {
            LOG_ERROR << "message is empty!!!" << std::endl;
        }

        std::cin.ignore(1024, '\n');;
    }

    ::close(iSockFd);

    return (0);
}