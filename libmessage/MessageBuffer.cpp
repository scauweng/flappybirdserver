#include "MessageBuffer.h"
#include <arpa/inet.h>
#include <iostream>
#include <cstdio>
using namespace std;
MessageBuffer::MessageBuffer(){
	from=to=0;
}

void MessageBuffer::addBuffer(char *new_buffer,int buffer_len){
	for(int i=0;i<buffer_len;i++){
		buffer[to]=new_buffer[i];
		to=(to+1)%MAXSIZE;
	}
}

void MessageBuffer::debugPrint(char *message,int len){
	cout<<"MessageBuffer::"<<len<<endl;
    cout<<"--------";
    for(int i=0;i<len;i++)printf("%c",message[i]);
    cout<<"--------";
    cout<<endl<<endl;
}

bool MessageBuffer::getPackage(char *package,int &buffer_len){
    cout<<"now get Package:"<<endl;
	if((to-from+MAXSIZE)%MAXSIZE<4)return false;
	//copy first 4 bytes in buffer to "len".
	cout<<"before:"<<from <<" "<<to<<" "<<buffer_len<<endl;
	int len=0;
	char *ptr=(char *)&len;
	for(int i=0;i<4;i++)ptr[i]=buffer[(from+i)%MAXSIZE];
	debugPrint(ptr,4);
	len=ntohl(len);//transform network bytes to local bytes
	cout<<"len:"<<len<<endl;

	if((to-from+MAXSIZE)%MAXSIZE<4+len)return false;  // not a full package
	buffer_len=len+4;
	for(int i=0;i<buffer_len;i++){
		package[i]=buffer[(from+i)%MAXSIZE];
	}
	debugPrint(package,buffer_len);
	package[buffer_len]='\0';
	from=(from+buffer_len)%MAXSIZE;
	cout<<"end:"<<from<<" "<<to<<" "<<buffer_len<<" "<<len<<endl;
    cout<<"get package true";
	return true;
}


void MessageBuffer::updateFrom(int len){
	to=(to-len+MAXSIZE)%MAXSIZE;
}

bool MessageBuffer::isEmpty(){
	if(to==from)return true;
	else return false;
}

bool MessageBuffer::getSendPackage(char *package,int &buffer_len){
    cout<<"now getSendPackage"<<endl;
	buffer_len=(to-from+MAXSIZE)%MAXSIZE;
	for(int i=0;i<buffer_len;i++){
		package[i]=buffer[(from+i)%MAXSIZE];
	}
    from=(from+buffer_len)%MAXSIZE;
    cout<<"get SendPackage true"<<endl;
    return 1;
}

bool MessageBuffer::getStatus(char *package,int &fd){   //after call getPackage, get fd
	/*
	int len=0;
	char *ptr=(char *)&len;
	for(int i=0;i<4;i++)ptr[i]=buffer[(from+i)%MAXSIZE];
	len=ntohl(len);//transform network bytes to local bytes
	*/
	fd=0;
	char *ptr=(char *)&fd;
    for(int i=0;i<4;i++)ptr[i]=package[i+4];
    fd=ntohl(fd);
    printf("get fd is %d\n",fd);
	return true;
}

bool MessageBuffer::addFdHead(char *package,int fd,int &buffer_len){  //
	for(int i=buffer_len-1;i>=0;i--)package[i+8]=package[i];
	addValue(package,htonl(buffer_len+4));
	addValue(package+4,htonl(fd));
	//cout<<fd<<" "<<buffer_len+4<<endl;
    printf("addFdHead:\n");
	buffer_len+=8;
    debugPrint(package,buffer_len);
	return true;
}

bool MessageBuffer::delFdHead(char *package,int &buffer_len){    //
	for(int i=0;i<buffer_len-8;i++)package[i]=package[i+8];
	buffer_len-=8;
    printf("delFdHead and:\n");
    debugPrint(package,buffer_len);
}


bool MessageBuffer::addLenHead(char *package,int &buffer_len){
	for(int i=buffer_len-1;i>=0;i--){
		package[i+4]=package[i];
	}
    printf("addLenHead");
	addValue(package,htonl(buffer_len));
	buffer_len+=4;
	return 1;
}

bool MessageBuffer::delLenHead(char *package,int &buffer_len){    //
    printf("delLenhead");
	for(int i=0;i<buffer_len;i++)package[i]=package[i+4];
	buffer_len-=4;
	return 1;
}

int MessageBuffer::leftSize(){
	return MAXSIZE-1-(to-from+MAXSIZE)%MAXSIZE;
}
