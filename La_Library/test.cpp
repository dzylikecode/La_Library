#define  _WINSOCK_DEPRECATED_NO_WARNINGS

#include <string.h>
#include <process.h>
#include "Platform.h"

UDPCLIENT client;

MSGPV4 serveraddr;
char sendData[256];
char recvData[256];
int isrunning = 1;

//接收消息的函数，单独一个线程中运行
void receive_msg(void* message = nullptr)
{
	while (1)
	{
		//接收来自聊天对方的消息
		int ret = client.Receive(serveraddr, recvData, 256);
		if (ret > 0)
		{
			recvData[ret] = '\0';					//填充字符串结束符'\0'
			printf("来自%s的新消息：%s\n", serveraddr.GetIP(), recvData);

			//如果消息是bye，则退出聊天
			if (!strcmp(recvData, "bye"))
			{
				isrunning = 0;
				printf("系统退出！");

				return;
			}
		}
	}
}

int main(int argc, char* argv[])
{
	CONSOLE console;
	console.Create();
	console.SetTitle(TEXT("基于UDP的简单聊天程序客户端"));
	console.resize(80, 25);
	console.Clear();
	printf("天天聊天室欢迎你！\n\n");

	client.Create(nullptr, 6666);


	MSGPV4 server("127.0.0.1", 6666);

	//启动接收消息线程
	_beginthread(receive_msg, 0, NULL);

	while (1)
	{
		if (!isrunning)
		{
			break;
		}

		//从键盘接收要发送的消息
		gets_s(sendData);
		//发送聊天消息
		client.Send(server, sendData, 256);


		//如果是bye，则退出聊天
		if (!strcmp(recvData, "bye"))
		{
			isrunning = 0;
			printf("系统退出！");
			break;
		}
	}

	//结束接收消息的线程
	_endthread();

	return 0;
}
