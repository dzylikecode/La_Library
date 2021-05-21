#define _CRT_SECURE_NO_WARNINGS
#include "Platform.h"
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <process.h>

//没法超过256，要不然无法对应端无法接受到
const int dataBuffer = 256;
UDPSEVER server;
ASTRING recvData(dataBuffer);
bool bRun = true;
MSGPV4 client;

//接收消息的函数，单独一个线程中运行
void receive_msg(void* message = nullptr)
{
	
	while (1)
	{
		//接收来自聊天对方的消息
		int ret = server.Receive(client, recvData, dataBuffer);
		if (ret > 0)
		{
			recvData[ret] = '\0';					//填充字符串结束符'\0'
			printf("来自%s的新消息：%s\n", client.GetIP(), recvData);

			//如果消息是bye，则退出聊天
			if (recvData == "bye")
			{
				bRun = false;
				printf("系统退出！");

				return;
			}
		}
	}
}

int main(int argc, char* argv[])
{
	server.Create(nullptr, 6666);

	//启动接收消息线程
	_beginthread(receive_msg, 0, nullptr);
	ASTRING sendData(dataBuffer);
	while (1)
	{
		if (!bRun)
		{
			break;
		}

		//从键盘接收要发送的消息
		gets_s((char*)sendData, dataBuffer);
		//发送聊天消息
		server.Send(client, sendData, dataBuffer);

		//如果是bye，则退出聊天
		if (recvData == "bye")
		{
			bRun = false;
			printf("系统退出！");
			break;
		}
	}

	//结束接收消息的线程
	_endthread();

	return argc;
}