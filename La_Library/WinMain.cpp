#define _CRT_SECURE_NO_WARNINGS
#include "Platform.h"
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <process.h>



int main(int argc, char* argv[])
{
	TCPCLIENT client;
	client.Create("127.0.0.1", 8888);

	//构造要发送的数据包
	const char* sendData = "你好，TCP服务器，我是客户端。\n";

	client.Send(sendData, strlen(sendData));

	char recvData[256];
	int ret = client.Receive(recvData, 256);
	if (ret > 0)
	{
		recvData[ret] = '\0';					//填充字符串结束符'\0'
		printf("服务器回应：%s", recvData);		//输出收到的数据包消息
	}

	return argc;
}