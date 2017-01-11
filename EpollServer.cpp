#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/types.h>
#include <map>
#include <utility>
#include <vector>
#include <fcntl.h>
#include "libmessage/MessageBuffer.h"
#include "libmessage/Message.h"
#define LOG_DEBUG std::cout << __FILE__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|DEBUG|"
#define LOG_ERROR std::cout << __FILE__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|ERROR|"
#define IPADDRESS   "0.0.0.0"
#define PORT        8889
#define MAXSIZE     1024
#define LISTENQ     3
#define FDSIZE      1000
#define EPOLLEVENTS 100
using namespace std;

#define x first
#define y second

typedef pair<int,int> pii;
vector<int> Fds;
int logicServerFd;
MessageBuffer ReadBuffer[MAXSIZE];
MessageBuffer WriteBuffer[MAXSIZE];
const char token[]="just a token";

//函数声明
//创建套接字并进行绑定
static int socket_bind(const char* ip,int port);
//IO多路复用epoll
static void do_epoll(int listenfd);
//事件处理函数
static void	handle_events(int epollfd,struct epoll_event *events,int num,int listenfd,char *buf);
//处理接收到的连接
static void handle_accpet(int epollfd,int listenfd);
//读处理
static void do_read(int epollfd,int fd,char *buf);
//写处理
static void do_write(int epollfd,int fd,char *buf);
//添加事件
static void add_event(int epollfd,int fd,int state);
//修改事件
static void modify_event(int epollfd,int fd,int state);
//删除事件
static void delete_event(int epollfd,int fd,int state);


void init(){
    Fds.clear();
}

int main(int argc,char *argv[])
{
    init();
    Fds.clear();
    int  listenfd;
    listenfd = socket_bind(IPADDRESS,PORT);
    listen(listenfd,LISTENQ);                                                   //listen  the second parameter is the max number of Client can connect
    fcntl(listenfd, F_SETFL, fcntl(listenfd, F_GETFD, 0)|O_NONBLOCK);
    do_epoll(listenfd);
    return 0;
}

static int socket_bind(const char* ip,int port)
{
    int  listenfd;
    struct sockaddr_in servaddr;
    listenfd = socket(AF_INET,SOCK_STREAM,0);                                    //socket
    if (listenfd == -1)
    {
        perror("socket error:");
        exit(1);
    }
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET,ip,&servaddr.sin_addr);
    servaddr.sin_port = htons(port);                  // htons for 16bits  htonl for 32bits
    int flag = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));
    if (bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) == -1)       //bind
    {
        perror("bind error: ");
        exit(1);
    }
    return listenfd;
}

static void do_epoll(int listenfd)
{
    int epollfd;
    struct epoll_event events[EPOLLEVENTS];
    int ret;
    char buf[MAXSIZE];
    memset(buf,0,MAXSIZE);
    //创建一个描述符
    epollfd = epoll_create(FDSIZE);                                 //create epoll,neet to close() after use
    //添加监听描述符事件
    add_event(epollfd,listenfd,EPOLLIN);                            //can to read
    cout<<"begin to epoll_wait"<<endl;
    for ( ; ; )													    //while to loop
    {
        //获取已经准备好的描述符事件
        ret = epoll_wait(epollfd,events,EPOLLEVENTS,3000);               //-1 to (non block)
        //cout<<"ret is:"<<ret<<endl;
        handle_events(epollfd,events,ret,listenfd,buf);                // a queue to handle

    }
    close(epollfd);                                                 //to close
}

static void handle_events(int epollfd,struct epoll_event *events,int num,int listenfd,char *buf)   //listenfd is local server id;
{
    //进行选好遍历
    for (int i = 0;i < num;i++)
    {
        int fd = events[i].data.fd;
        //根据描述符的类型和事件类型进行处理
        if ((fd == listenfd) &&(events[i].events & EPOLLIN))             //?   accept a new connection
            handle_accpet(epollfd,listenfd);
        else if (events[i].events & EPOLLIN)
            do_read(epollfd,fd,buf);
        if (events[i].events & EPOLLOUT)
            do_write(epollfd,fd,buf);
    }
}

// core code begin
static void handle_accpet(int epollfd,int listenfd)
{
    int clifd;
    struct sockaddr_in cliaddr;
    socklen_t  cliaddrlen;
    clifd = accept(listenfd,(struct sockaddr*)&cliaddr,&cliaddrlen);       //accept a clifd

    if (clifd == -1)
        perror("accpet error:");
    else
    {
        Fds.push_back(clifd);
        printf("new fd is %d\n",clifd);
        printf("accept a new client: %s:%d\n",inet_ntoa(cliaddr.sin_addr),cliaddr.sin_port); //inet_ntoa
        printf("xxx is %s\n",inet_ntoa(cliaddr.sin_addr));
        // if cliaddr.sin_addr is"127.0.0.1" ,than it is a Login Server
        if(string(inet_ntoa(cliaddr.sin_addr))==string("0.0.0.0")||string(inet_ntoa(cliaddr.sin_addr))==string("127.0.0.1")){
            printf("a logicServer connect ....\n");
            add_event(epollfd,clifd,EPOLLIN);
            logicServerFd=clifd;
            return;
        }
	    else {
	    	printf("a client connect ....\n");
            add_event(epollfd,clifd,EPOLLIN);
	    }
    }
}

void print(char *message,int messageLen){
    cout<<"len is:"<<messageLen<<endl;
    for(int i=0;i<messageLen;i++)printf("index %d:%d\n",i,message[i]);
}

void do_read(int epollfd,int fd,char *buf)
{
    //if receive  a logicServer message  ,try to send message to player's client
    //else if receive a client message, try to send message to logicServer

    int nread = read(fd,buf,MAXSIZE);
    printf("fd is %d and nread is %d\n",fd,nread);
    if (nread == -1)
    {
        printf("errno %d\n",errno);
        if(errno!=EINTR){
            perror("read error:");
            close(fd);
            delete_event(epollfd,fd,EPOLLIN);
        }
    }
    else if (nread == 0)
    {
        fprintf(stderr,"client close.\n");
        close(fd);
        delete_event(epollfd,fd,EPOLLIN);
    }
    else
    {
        printf("read message is : %s",buf);
        if(fd==logicServerFd){          //read message from logicServer
            ReadBuffer[fd].addBuffer(buf,nread);            //add buffer

            char message[128];
            int messageLen;
            int clientFd;
            while(ReadBuffer[fd].getPackage(message,messageLen)){
                message[messageLen]='\0';
                printf("receive message from logicServer:%s\n",message);
                ReadBuffer[fd].getStatus(message,clientFd);
                ReadBuffer[fd].delFdHead(message,messageLen);     //just  del FdHead,and Send to client
                if(WriteBuffer[clientFd].isEmpty()){
                    WriteBuffer[clientFd].addBuffer(message,messageLen);
                    add_event(epollfd,clientFd,EPOLLOUT);
                }
                else WriteBuffer[clientFd].addBuffer(message,messageLen);
            }
        }

        else {                        //read message from Client
            ReadBuffer[fd].addBuffer(buf,nread);
            char message[128];
            int messageLen;
            while(ReadBuffer[fd].getPackage(message,messageLen)){
                message[messageLen]='\0';
                printf("receive message from a client:%s\n",message);
                print(message,messageLen);
                ReadBuffer[fd].addFdHead(message,fd,messageLen);     //just add FdHead,and Send to logic
                print(message,messageLen);
                if(WriteBuffer[logicServerFd].isEmpty()){
                    printf("logic ServerFd isEmpty()");
                    WriteBuffer[logicServerFd].addBuffer(message,messageLen);
                    modify_event(epollfd,logicServerFd,EPOLLIN|EPOLLOUT);
                }
                else {
                    printf("logicServerFd is not Empty()");
                    WriteBuffer[logicServerFd].addBuffer(message,messageLen);
                }
            }
        }
    }
}

void printDebug(char* msg, int len)
{
    cout << "++++++"<< len << "+++++" << endl;
    for (int i = 0; i < len; i++) {
        printf("%02x",msg[i]);
    }
    cout << "+++++++++++++++++++" << endl;
}

static void do_write(int epollfd,int fd,char *buf)
{
	printf("write : id is : %d\n",fd);
    char sendMessage[MAXSIZE+1];
    int sendMessageLen;
    WriteBuffer[fd].getSendPackage(sendMessage,sendMessageLen);
    if(sendMessageLen==0)return;
    printDebug(sendMessage, sendMessageLen);
    int nwrite=write(fd,sendMessage,sendMessageLen);

    printf("send message is: %s to %d\n",buf,fd);
    printf("sendMessageLen is %d and nwrite is %d\n",sendMessageLen,nwrite);
    if (nwrite == -1)
    {
        perror("write error:");
        close(fd);
        delete_event(epollfd,fd,EPOLLOUT);
    }
    else if(nwrite==sendMessageLen)
    {
        delete_event(epollfd,fd,EPOLLOUT);
    }
    else {
        WriteBuffer[fd].updateFrom(sendMessageLen-nwrite);
    }
}//end


static void add_event(int epollfd,int fd,int state)
{
    printf("add epollfd and fd is %d\n",fd);
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&ev);
}

static void delete_event(int epollfd,int fd,int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,&ev);
}

static void modify_event(int epollfd,int fd,int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&ev);
}
