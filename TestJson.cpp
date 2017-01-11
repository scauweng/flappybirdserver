#include "MessageBuffer.h"

Message::Message(){
}

Message::Message(char *msg,int len){
	string s=msg;
	int begin=0;
	int end=0;
	while(begin<len){
		for(end=begin;end<len&&msg[end]!=';';end++);
		solveField(s,begin,end);
		begin=end;
	}
}

Message::~Message(){

}

string Message::toString(){                                             //Create message 
	string res="";
	for(map<string,string>::iterator it=status.begin();it!=status.end();it++){
		res+=it->first;
		res+=':';
		res+=it->second;
		res+=';';
	}
	return res;
}

void solveField(string s,int begin,int end){
	int mid;
	for(mid=begin;mid<end&&s[mid]!=':';mid++);
	status[s.substr(begin,mid)]=s.substr(mid+1,end);
}

void Message::addKeyValue(char *key,char *value){
	status[key]=string(value);
}

bool Message::hasKey(char *key){
	return status.find(key)!=status.end();
}

void Message::delKey(char *key){
	status.erase(status.find(key));
}

#ifndef MESSAGE_H
#define MESSAGE_H
#include <bits/stdc++.h>


class Message{
private:
	map<string,string> status;
	char msgstr[1024];
public:
	Message();
	Message(char *msg,int len);
	~Message();
	string toString();
	void solveField(const char *msg,int len);
	void addKeyValue(char *key,char *value);
	bool hasKey(char *key);
	void delKey(char *key);

	template<typename T>
	static void addValue(char *buffer,T val);
};

template<typename T>
void Message::addValue(char *buffer,T val){
	T *ptr=(T *)buffer;
	*ptr=val;
}

#endif



#include "MessageBuffer.h"
#include <arpa/inet.h>

MessageBuffer::MessageBuffer(){
	from=to=0;
}

void MessageBuffer::addBuffer(char *new_buffer,int buffer_len){
	for(int i=0;i<buffer_len;i++){
		buffer[to]=new_buffer[i];
		to=(to+1)%MAXSIZE;
	}
}

bool MessageBuffer::getPackage(char *package,int &buffer_len){       
	if((to-from+MAXSIZE)%MAXSIZE<4)return false;
	//copy first 4 bytes in buffer to "len".
	int len=0;
	char *ptr=(char *)&len;
	for(int i=0;i<4;i++)ptr[i]=buffer[(from+i)%MAXSIZE];
	len=ntohl(len);//transform network bytes to local bytes
	
	if((to-from+MAXSIZE)%MAXSIZE<4+len)return false;  // not a full package
	buffer_len=len+4;
	for(int i=0;i<buffer_len;i++){
		package[i]=buffer[(from+i)%MAXSIZE];
	}
	package[buffer_len]='\0';
	from=(from+buffer_len)%MAXSIZE;
	return true;
}


void MessageBuffer::updateFrom(int len){
	to=(to-len+MAXSIZE)%MAXSIZE;
}

bool MessageBuffer::isEmpty(){
	return to==from;
}

bool MessageBuffer::getSendPackage(char *package,int &buffer_len){
	buffer_len=(to-from+MAXSIZE)%MAXSIZE;
	for(int i=0;i<buffer_len;i++){
		package[i]=buffer[(to+i)%MAXSIZE];
	}
	to=(to+buffer_len)%MAXSIZE;
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
	for(int i=0;i<4;i++)ptr[i]=buffer[(from+4+i)%MAXSIZE];
	fd=ntohl(fd);
	return true;
}

bool MessageBuffer::addFdHead(char *package,int fd,int &buffer_len){  //   
	for(int i=buffer_len-1;i>=0;i--)package[i+8]=package[i];
	addValue(package,htonl(buffer_len+4));
	addValue(package+4,htonl(fd));
	buffer_len+=8;
	return true;
}

bool MessageBuffer::delFdHead(char *package,int &buffer_len){    //
	for(int i=0;i<buffer_len;i++)package[i]=package[i+8];
	buffer_len-=8;
}	


bool MessageBuffer::addLenHead(char *package,int &buffer_len){
	for(int i=buffer_len-1;i>=0;i--){
		package[i+4]=package[i];
	}
	addValue(package,htonl(buffer_len));
	return 1;
}

bool MessageBuffer::delLenHead(char *package,int &buffer_len){    //
	for(int i=0;i<buffer_len;i++)package[i]=package[i+4];
	buffer_len-=4;
} 

int MessageBuffer::leftSize(){
	return MAXSIZE-1-(to-from+MAXSIZE)%MAXSIZE;
}