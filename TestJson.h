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


#ifndef MESSAGEBUFFER_H
#define MESSAGEBUFFER_H

class MessageBuffer{
private:
	template<typename T>
	static void addValue(char *buffer,T val);
	static const int MAXSIZE=1024;
	char buffer[MAXSIZE+1];
	int from,to;
public:
	MessageBuffer();
	void addBuffer(char *new_buffer,int buffer_len);
	bool getPackage(char *package,int &buffer_len);
	bool getStatus(char *package,int &fd);
	bool addLenHead(char *package,int &buffer_len);
	bool delLenHead(char *package,int &buffer_len);
	bool addFdHead(char *package,int fd,int &buffer_len);
	bool delFdHead(char *package,int &buffer_len);
	bool getSendPackage(char *package,int &buffer_len);
	void updateFrom(int len);
	bool isEmpty();
    static bool CreateMessage(char *message,int pId,int option,int to,int &messageLen);
	int leftSize();
};

template<typename T>
void MessageBuffer::addValue(char *buffer,T val){
	T *ptr=(T *)buffer;
	*ptr=val;
}

#endif
