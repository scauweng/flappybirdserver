#include "MyMysql.h"
#include <stdio.h>
#include <string.h>
MyMysql::MyMysql(){
	mysql_init(&mysql);
}

MyMysql::~MyMysql(){
	mysql_close(&mysql);
}

bool MyMysql::connect(char *userName,char *passWord,char databaseName){
	if(!mysql_real_connect(&mysql,"localhost","root","6557632","player",0,NULL,0)){
		printf("Failed to connect to Mysql!\n");
		return 0;
	}
	else {
		printf("Connected to Mysql successfully!\n");
	}
}

bool MyMysql::query(char *querystr){
	int flag=mysql_real_query(&mysql,querystr,(unsigned int)strlen(querystr));
	if(flag){
		printf("Query failed!\n");
		return false;
	}else{
		printf("[%s] made...\n",querystr);
		res=mysql_store_result(&mysql);
		return true;
	}
}

