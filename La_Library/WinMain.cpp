#define _CRT_SECURE_NO_WARNINGS
#include "Platform.h"
#include "La_File.h"
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <process.h>
const int MAX_SIZE = 10240;
void ReportConnectInfo(SOCKET threadMsg);
TCPSERVER tcpServer;
int numThread = 0;
void DealClient(void* param);

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		printf("使用方式： server 端口\n");
		exit(1);
	}

	int port = atoi(argv[1]);
	
	//winsock2.h中有#define SOMAXCONN	0x7fffffff /* (5) in WinSock1.1 */
	if (!tcpServer.Create("0.0.0.0", port, SOMAXCONN))
	{
		printf("服务器启动失败！\n");
	}
	printf("服务器已启动，监听于端口%d！\n", port);

	MSGPV4 client; //应该说是 temp 一定要是类似全局的声明周期， 最后调用析构
	while (1)
	{
		tcpServer.GetClient(client);

		printf("客户端（%s：%d）已连接！\n", client.GetIP(), client.GetPort());

		//启动线程处理一个客户端的上传
		_beginthread(DealClient, 0, (void*)&client);
		Sleep(1000);

		//输出双方的IP地址及端口信息
		ReportConnectInfo(client.GetSocket());
	}

	while (numThread)
		Sleep(20);

	_endthread();
	return argc;
}

//输出连接双方的IP地址和端口信息
void ReportConnectInfo(SOCKET threadMsg)
{
	MSGPV4 server;
	GetServerMSG(server, threadMsg);
	MSGPV4 client;
	GetClientMSG(client, threadMsg);
	printf("服务器：（%s：%d）\n", server.GetIP(), server.GetPort());
	printf("客户端：（%s：%d）\n", client.GetIP(), client.GetPort());
}

//需要申请东西的类多少有一些奇怪
//像 client 我在main 里面必须和 main 同周期
//若是放在 while 循环，每次 client 都会关闭 socket
//所以这样传给线程并没有什么用
//于是 client 必须放在外面
//我相当于让 client 代理 socket ，当多个 client 代理时，应当注意
//而且这就是要浅复制，因为 socket 就是一个指针
//处理客户端文件上传的线程
void DealClient(void* param)
{
	numThread++;
	//param 里面的值需要保留下来，而不是引用，因为 param 随时都在变
	MSGPV4 client = *(MSGPV4*)param;
	printf("\n\n\n创建新线程成功！\n\n\n");
	char filename[MAX_PATH];
	char data[MAX_SIZE];
	char ch;
	STDFILE file;
	printf("接收文件名\n");

	memset((void*)filename, 0, sizeof(filename));
	int i;
	for (i = 0; i < sizeof(filename); i++)
	{
		if (tcpServer.Receive(client, &ch, 1) != 1)
		{
			printf("接收失败或客户端已关闭连接\n");
			numThread--;
			return;
		}
		if (ch == 0)
		{
			break; //得益于 filename 一开始是清空的，故不用复制 \0
		}
		filename[i] = ch;
	}
	if (i == sizeof(filename))
	{
		printf("文件名过长\n");
		numThread--;
		return;
	}
	printf("文件名%s\n", filename);

	if (!file.Open(filename, "wb"))
	{
		printf("无法以写方式打开文件\n");
		numThread--;
		return;
	}
	printf("接收文件内容");
	memset((void*)data, 0, sizeof(data));
	while (1)
	{
		//receive 是会阻塞的
		int i = tcpServer.Receive(client, data, sizeof(data));
		putchar('.');//用来表示进度
		if (i == SOCKET_ERROR)
		{
			printf("\n接收失败，文件可能不完整\n");
			break;
		}
		else if (i == 0)
		{
			printf("\n接收成功\n");
			break;
		}
		else
		{
			fwrite((void*)data, 1, i, file.GetFile());
		}
	}
	numThread--;
}