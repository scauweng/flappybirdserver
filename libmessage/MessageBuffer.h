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
	void debugPrint(char *message,int len);
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