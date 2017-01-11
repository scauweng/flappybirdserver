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
#include <fcntl.h>
#include <map>
#include <utility>
#include <vector>
#include <time.h>
#include "Game.h"
#include <sstream>
#include "libmessage/Message.h"
#include "libmessage/MessageBuffer.h"

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
#define theOther(x) flappyBird.playerFd[flappyBird.id[x]^1]
char ip[]="0.0.0.0";
const int port=8889;
int GatewayFd;
Game flappyBird;
MessageBuffer WriteBuffer;
MessageBuffer ReadBuffer;
char package[1024];
int packageLen;


int Connect(const char* server_ip){
	int a1,a2,a3,a4;
	if(NULL==server_ip
		||sscanf(server_ip,"%d.%d.%d.%d",&a1,&a2,&a3,&a4)!=4){
		LOG_ERROR<<"socket error|msg:"<<"invalid server_ip"<<std::endl;
	}

	int sockfd=::socket(AF_INET,SOCK_STREAM,0);
	if(-1==sockfd){
		LOG_ERROR<<"socket error|msg:"<< ::strerror(errno)<<std::endl;
		return -1;
	}

	struct sockaddr_in servaddr;
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(port);
	servaddr.sin_addr.s_addr=htonl((a1<<24)|(a2<<16)|(a3<<8)|a4);
	if(::connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr))==-1){
		LOG_ERROR<<"connect to server(" <<server_ip <<") error|msg: "<<::strerror(errno)<<std::endl;
		return -1;
	}
	fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK);              //set nonblock
	return sockfd;
}

void SendTo(const char *message,int messageLen,int clientFd){
    printf("try to send a message:%s\n to %d\n\n",message,clientFd);
	char msg[1024];
	memcpy(msg,message,messageLen);
	int len=messageLen;
	WriteBuffer.addLenHead(msg,len);
	WriteBuffer.addFdHead(msg,clientFd,len);
    cout<<"add WriteBuffer"<<endl;
	WriteBuffer.addBuffer(msg,len);
}

string tostring(int num){
    stringstream ss;
    ss<<num;
    string res;
    res=ss.str();
    cout<<"res is :"<<res<<endl;
	return res;
}

void DoWork(char *s,int len){          //do work for receive message from gateway service
	ReadBuffer.addBuffer(s,len);
	while(ReadBuffer.getPackage(package,packageLen)){
		int clientFd;
		ReadBuffer.getStatus(package,clientFd);
        ReadBuffer.delFdHead(package,packageLen);
        package[packageLen]='\0';
        printf("receive from gateway server :%s\n",package);
		Message msg(package+4,packageLen-4);
		string option=msg.getValue("option");

        printf("receive message is :\n");
        cout<<"msg is:"<<msg.toString()<<endl;
        cout<<"option is:"<<option<<endl;
        cout<<endl<<endl;
		if(option=="begin"){
			flappyBird.prepare(clientFd);     // just a player prepare, when 2 player prepare, begin the game
			if(flappyBird.playerCnt){
				flappyBird.startGame();

				Message msg;
				msg.addKeyValue("option","begin");
				int randnum=rand();
				msg.addKeyValue("randnum",tostring(randnum));
				string smsg=msg.toString();
                const char *charmsg=smsg.c_str();
                int messageLen=smsg.length();
                cout<<"smsg is :"<<smsg<<endl;
				//Send "begin" to both player
				SendTo(charmsg,messageLen,clientFd);
				//SendTo(charmsg,messageLen,theOther(clientFd));
			}
		}
		else if(option=="jump"){
			Message msg;
			msg.addKeyValue("option","jump");
			msg.addKeyValue("user","me");
			string smsg=msg.toString();
			const char *charmsg=smsg.c_str();
			int messageLen=strlen(charmsg);
			SendTo(charmsg,messageLen,clientFd);

			msg.delKey("user");
			msg.addKeyValue("user","theOther");
			smsg=msg.toString();
			const char *charmsg2=smsg.c_str();
			int messageLen2=strlen(charmsg2);
			SendTo(charmsg2,messageLen2,theOther(clientFd));
		}
		else if(option=="die"){
			Message msg;
			msg.addKeyValue("option","die");
			msg.addKeyValue("user","me");
			string smsg=msg.toString();
			const char *charmsg=smsg.c_str();
			int messageLen=strlen(charmsg);
			SendTo(charmsg,messageLen,clientFd);

			msg.delKey("user");
			msg.addKeyValue("user","theOther");
			smsg=msg.toString();
			const char *charmsg2=smsg.c_str();
			int messageLen2=strlen(charmsg2);
			SendTo(charmsg2,messageLen2,theOther(clientFd));
		}
	}
}
void print(char *msg,int len){
    cout<<"len:"<<endl;
    for(int i=0;i<len;i++){
        printf("index %d is %d\n",i,msg[i]);
    }
}
int main(){
	GatewayFd=Connect(ip);
	char buffer[40960];
	//flappyBird.Init();
	while(1){ 											//use while loop to send and receive
		if(!WriteBuffer.isEmpty()){
            cout<<"WriteBuffer is not empty"<<endl;
			char message[1024];
			int messageLen;
			WriteBuffer.getSendPackage(message,messageLen);
			int nwrite=::write(GatewayFd,buffer,messageLen);
            printf("messageLen is %d and nwrite is %d\n",messageLen,nwrite);
			if(nwrite<0){
				LOG_ERROR<<"Write to Gateway server error|msg: "<<::strerror(errno)<<std::endl;
				return 0;
			}
			else if(nwrite==0){
				LOG_ERROR<<"write nothing to Gateway server "<<std::endl;
				return 0;
			}
			else if(nwrite<messageLen){
				WriteBuffer.updateFrom(messageLen-nwrite);
			}
		}

		ssize_t ReadLen=::read(GatewayFd,buffer,sizeof(buffer)-1);
		//cout<<"ReadLen:"<<ReadLen<<endl;
		if(ReadLen<0){
			//LOG_ERROR<<"read from Gateway server error|msg: "<<::strerror(errno)<<std::endl;
			continue;
			//return 0;
		}else if(0==ReadLen){
            continue;
			LOG_ERROR<<"read no message from Gateway server"<<std::endl;
		}else{
			buffer[ReadLen]='\0';
            print(buffer,ReadLen);
            //printf("len is %d have recevie : \n%s\n",ReadLen,buffer);
			DoWork(buffer,(int)ReadLen);
			LOG_DEBUG<<"read message from server|msg:"<<buffer<<std::endl;
		}
	}
	::close(GatewayFd);
	return 0;
}
