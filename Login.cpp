#include "MyMysql.h"
#include <bits/stdc++.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/types.h>
#define PORT 8123
#define ip "0.0.0.0"

const char token[]="just a token";

#define LOG_DEBUG std::cout << __FILE__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|DEBUG|"
#define LOG_ERROR std::cout << __FILE__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|ERROR|"
using namespace std;


int socket_bind(){
	int  listenfd;
    struct sockaddr_in servaddr;
    listenfd = socket(AF_INET,SOCK_STREAM,0);                                    //socket
    if (listenfd == -1)
    {
        perror("socket error:");
        exit(1);
    }
    int iFlag=1;
    ::setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&iFlag, sizeof(iFlag));
    bzero(&servaddr,sizeof(servaddr));	
    servaddr.sin_family = AF_INET;  
    inet_pton(AF_INET,ip,&servaddr.sin_addr);   
    servaddr.sin_port = htons(port);                  // htons for 16bits  htonl for 32bits
    fcntl(listenfd, F_SETFL, fcntl(listenfd, F_GETFD, 0)|O_NONBLOCK);
    if (bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) == -1)       //bind
    {
        perror("bind error: ");
        exit(1);
    }
    return listenfd; 
}

int main(){
	MyMysql playerTable;
	int listenfd=socket_bind();
	for(;;){
		socklen_t uiClientAddrLen = sizeof(stClientAddr);
		int iClientFd = ::accept(iAcceptFd, (struct sockaddr*)&stClientAddr, &uiClientAddrLen);

		if(-1 == iClientFd)
		{
			LOG_ERROR << "accept client error|msg:" << ::strerror(errno) << std::endl;

			continue;
		}

        inet_ntop(AF_INET, &(stClientAddr.sin_addr), szClientIP, sizeof(szClientIP));
        LOG_DEBUG << "accept client:" << szClientIP << ":" << ntohs(stClientAddr.sin_port) << std::endl;

		ssize_t iRet = ::read(iClientFd, pReadBuffer, sizeof(pReadBuffer) - 1);
		if(-1 == iRet)
		{
			//read error
			LOG_ERROR << "read error|msg:" << ::strerror(errno) << std::endl;
		}
		else if(0 == iRet)
		{
			LOG_DEBUG << "client request to close connection" << std::endl;
		}
		else
		{
			pReadBuffer[iRet] = '\0';
			LOG_DEBUG << "recv client message|msg_len:" << iRet << "|msg_content:"
					  << pReadBuffer << std::endl;

			
		}

		::close(iClientFd);
	}
}