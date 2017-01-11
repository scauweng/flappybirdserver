#ifndef MESSAGE_H
#define MESSAGE_H
#include <bits/stdc++.h>
#include <map>
using namespace std;
class Message{
private:
	map<string,string> status;
	char msgstr[1024];
public:
	Message();
	Message(char *msg,int len);
	~Message();
	string toString();
	string getValue(const string key);
	void solveField(const string msg,int begin,int end);
	void addKeyValue(const string key,const string value);
	bool hasKey(const string key);
	void delKey(const string key);

	template<typename T>
	static void addValue(char *buffer,T val);
};

template<typename T>
void Message::addValue(char *buffer,T val){
	T *ptr=(T *)buffer;
	*ptr=val;
}

#endif
