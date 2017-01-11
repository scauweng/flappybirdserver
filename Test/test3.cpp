#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <map>
#include "libmessage/MessageBuffer.h"

using namespace std;
using namespace rapidjson;
map<string,string> status;

int main(){
	const char json[]="{\"hello\":\"world\"}";
	printf("json len is %d\n",strlen(json));

	printf("Original JSON:\n %s\n",json);
	Document document;
	char buffer[sizeof(json)];
	memcpy(buffer,json,sizeof(json));
	if(document.ParseInsitu(buffer).HasParseError())return 1;
	printf("\nParsing to document succeeded.\n");

	Value::MemberIterator hello=document.FindMember("hello");
	//assert(hello==document.MemberEnd());
	assert(hello->value.IsString());
	assert(strcmp("world",hello->value.GetString())==0); 


	for(Value::ConstMemberIterator itr=document.MemberBegin();itr!=document.MemberEnd();itr++)
		printf("%s\n%s\n\n",itr->name.GetString(),itr->value.GetString());
	

	return 0;
}
