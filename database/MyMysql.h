#include "mysql.h"

class MyMysql{
private:
	MYSQL mysql;
	MYSQL_RES *res;
	MYSQL_ROW row;

public:
	MyMysql();
 	~MyMysql();
	bool connect(char *userName,char *passWord,char databaseName);
	void query(char *querystr);
}