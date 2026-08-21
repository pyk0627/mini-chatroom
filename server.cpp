#include <iostream>
using namespace std;
int server_socket;
int main()
{
	if((server_socket=socket(AF_INET,SOCK_STREAM,0))==-1)
	{
		perror("socket: ");//先输出socket：，再输出具体的系统错误
		exit(-1);//直接终止程序
	}
	return 0;
}