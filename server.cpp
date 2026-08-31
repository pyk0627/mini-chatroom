#include <iostream>
#include <sys/socket.h>//提供socket函数
#include <cstring>//提供memset
#include <cstdlib>//提供exit,EXIT_FAILURE
#include <netinet/in.h>//提供sockaddr_in,INADDR_ANY
#include <arpa/inet.h>//提供htons
#define MAX_LEN 200
using namespace std;
struct clientinfo
{
	int id;
	string name;
	int socket;
	thread th;
};
vector<clientinfo> clients;
void shared_print(string str,bool endline=true);
int main()
{
	int server_socket;//创建服务端的套接字
	if((server_socket=socket(AF_INET,SOCK_STREAM,0))==-1)
	//AF_INET代表地址族使用IPv4
	//SOCK_STREAM表示面向连接的，可靠的字节流传输，对应TCP
	//0表示让系统自动选择默认协议
	{
		perror("socket");//先输出socket，再输出具体的系统错误
		exit(EXIT_FAILURE);//直接终止程序
	}

	struct sockaddr_in server;
	//该结构体，sockaddr_in，用来保存IPv4的地址信息
	memset(&server,0,sizeof(server));//先将里面的数据清零
	server.sin_family=AF_INET;//服务端的地址族使用的是IPv4
	server.sin_port=htons(10000);
	//将服务端的端口号设置为10000
	//端口号用来区分一台主机的不同服务
	server.sin_addr.s_addr=INADDR_ANY;
	//internet address any
	//INADDR_ANY表示这个服务端监听本机的所有IPv4网络接口

	if((bind(server_socket,(struct sockaddr *)&server,sizeof(struct sockaddr_in)))==-1)
	//bind将刚才创建的socket与刚才编写的ip地址和端口绑定在一起
	//第一个参数，server_socket表示要绑定的套接字
	//第二个参数，(struct sockaddr *)&server,
	//这里的server就是刚才编写的ip地址和端口
	//(struct sockaddr *)是强制类型转换为通用地址结构体
	//第三个参数，sizeof(struct sockaddr_in)告诉你传入的地址有多大
	{
		perror("bind error");
		exit(EXIT_FAILURE);
	}

	//之前的socket相当于创建了一个门
	//bind相当于给门安装了一个门牌号
	//listen相当于营业开张
	if(listen(server_socket,8)==-1)
	//第一个参数，server_socket，代表哪扇门要开张
	//第二个参数，表示最多可以几个网络请求在排队
	{
		perror("listen error");
		exit(EXIT_FAILURE);
	}

	struct socketaddr_in client;
	//用来保存接待的客户的信息
	int client_socket;
	//之前的server_socket是门卫
	//接待一个客户之后，用一个新的套接字，也就是这个client_socket
	//来与客户聊天，门卫继续接待客户
	socklen_t len=sizeof(client);
	//得到client的大小
	cout<<"\n\t ======Welcome to the chatroom ======"<<endl;

	int seed=0;
	while(1)
	{
		if((client_socket=accept(server_socket,(struct sockaddr *)&client,&len))==-1)
		//accept
		//第一个参数我要从哪个套接字来接受客户端的连接
		//第二个参数是客户端的结构体地址，当客户端连接成功后，会把客户端信息放到client中
		//第三个参数，告诉服务端客户端有多大
		//返回一个套接字，用一个新的套接字来与客户端沟通，门卫继续接待下一个
		{
			perror("accept error");
			exit(EXIT_FAILURE);
		}
		seed++;//种子，用来区分，第1，2，3...个请求
		thread t(handle_client,client_socket,seed);
		//thread：线程，用来创建一个子线程
		//子线程的名字是t
		//调用handle_client函数
		//将client_socket，seed传入handle_client中

		//client和client_socket的区别
		//client相当于身份证，用来识别是谁
		//client_socket是通信管道，用来通信
	}
	return 0;
}

void set_name(int id,char name[])
{
	for(int i=0;i<clients.size();i++)
	{
		if(clients[i].id==id)
		{
			client[i].name=string(name);
		}
	}
}

int broadcast_message(int num,int id)
{//广播给当前客户端之外的客户
	for(int i=0;i<clients.size();i++)//遍历已连接的客户端
	{
		if(clients[i].id!=id)//不是当前的客户端
		{
			send(clients[i].socket,&num,sizeof(num),0);
			//clients[i].socket，要发送的第i个客户端
			//&num,找到num的地址
			//发送num大小的数据
			//0表示普通发送
		}
	}
}

int broadcast_message(string message,int id)
{
	char temp[MAX_LEN];
	//将string类型的message转化为固定长度的temp，char数组类型的
	//应用层协议
	strcpy(temp,message.c_str());
	//.c_str()，返回一个末尾是'\0'的字符数组的指针
	//strcpy(),将右边的复制到左边
	for(int i=0;i<clients.size();i++)
	{
		if(clients[i].id!=id)
		{
			send(clients[i].socket,temp,sizeof(temp),0);
		}
	}
}

void shared_print(string str,bool endline=true)
{
	lock_guard<mutex> guard(cout_mtx);
	//在当前作用域内给cout上锁
	//保护cout输出，防止内容交错

	//mutex cout_mtx
	//定义了一个互斥锁变量，变量名是cout_mtx

	//lock_guard，自动管理锁的工具
	//lock_guard是一个模板类
	//lcok_guard<mutex>，自动管理的锁是mutex(互斥锁)

	//lock_guard<mutex> guard，这里的guard是用模版类创建的对象名字

	//lock_guard<mutex> guard<cout_mtx>
	//创建了一个叫guard的锁管理器，用来管理互斥锁，cout_mtx
	//创建的时候自动给cout_mtx加锁，guard生命周期结束后自动解锁

	cout<<str;
	//再运行cout这行代码之前先考虑上一样的互斥锁
	if(endline)
		cout<<endl;
}
void handle_client(int client_socket,int id)
{
	char name[MAX_LEN],str[MAX_LEN];//MAX_LEN为200
	//TCP协议约定第一个发送到数据包是字符串
	recv(client_socket,name,sizeof(name),0);
	//从client_socket接收数据
	//存储到name中，
	//接受sizeof(name)多个数据
	//0代表默认阻塞模式
	//一直在这里等待接收数据
	set_name(id,name);//利用这个函数来保存昵称

	string welcome_message=string(name)+string("已经 加入");
	broadcast_message("!!!!",id);
	broadcast_message(id,id);//对当前id的其他id发送当前的id
	broadcast_message(welcome_message,id);
	//对当前id的其他id发送欢迎消息
	
	shared_print(welcome_message);//发送到当前服务端的终端上
}