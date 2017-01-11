#include "Message.h"
#include <iostream>
#include <cstdio>
using namespace std;

Message::Message(){
}

Message::Message(char *msg,int len){
    printf("init a new Message.");

    cout<<endl;
    cout<<"msg is:";
    for(int i=0;i<len;i++)printf("%c",msg[i]);
    cout<<endl<<endl;
    string s="";
    for(int i=0;i<len;i++)s+=msg[i];
	int begin=0;
	int end=0;
    while(begin<len){
        end=begin;
        while(msg[end]!=';')end++;
        solveField(s,begin,end);
        begin=end+1;
    }
}

Message::~Message(){

}

string Message::getValue(const string key){
	return status[key];
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

void Message::solveField(const string s,int begin,int end){

	int mid;
	for(mid=begin;mid<end&&s[mid]!=':';mid++);
    string key=s.substr(begin,mid);
    string value=s.substr(mid+1,end-mid-1);
    cout<<"key is:"<<key<<endl;
    cout<<"value is:"<<value<<endl;
	status[key]=value;
    cout<<"update and then status is :"<<toString()<<endl;
}

void Message::addKeyValue(const string key,const string value){
	status[key]=value;
    printf("addKeyValue done\n");
    cout<<key<<" "<<value<<endl;
    cout<<"update and then status is :"<<toString()<<endl;
}

bool Message::hasKey(const string key){
	return status.find(key)!=status.end();
}

void Message::delKey(const string key){
	status.erase(status.find(key));
}
