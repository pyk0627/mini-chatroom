#include <iostream>
#include <sys/socket.h>//提供socket(),SOCK_STREAM
#include <cstring>//提供memset
#include <cstdlib>//提供exit(-1)
#include <arpa/inet.h>//提供inet_addr
#include <netinet/in.h>//提供sockaddr_in,htons
#include <cstdio>//包含perror
//network internet internet
using namespace std;
int client_socket;
int main()
{
	if((client_socket=socket(AF_INET,SOCK_STREAM,0))==-1)
	{
		//AF_INET＝address family internet
		//AF_INET代表IPv4
		//SOCK_STREAM=socket stream
		//对应TCP
		//0代表默认协议
		perror("socket");
		exit(-1);
	}
	struct sockaddr_in server_addr;
	//用sockaddr_in结构体类型创建一个server_addr变量
	//用这个结构体来填写一些服务端的信息

	memset(&server_addr,0,sizeof(server_addr));
	//先把这个结构体清理干净，再填写信息
	server_addr.sin_family=AF_INET;//服务端的地址族类型
	server_addr.sin_port=htons(10000);//服务端的端口号
	//htons=host to network short
	//将主机字节序转化为网络字节序
	server_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	//要连接的服务端ip，也就是本机
	//结构体sockaddr_in中有一个结构体叫
	//sin_addr,里面有一个存储32位二进制的变量叫
	//s_addr，所以s_addr是真正存储IPv4的32个数字的变量
	return 0;
}
