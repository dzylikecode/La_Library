#define _CRT_SECURE_NO_WARNINGS
#include "La_NetWork.h"
#include "La_Windows.h"
#include "La_Shell.h"
#include <stdio.h>

int main(int argc, char* argv[])
{
	UDPSEVER server;
	server.Create(nullptr, 8888);
	MSGPV4 client;
	while (1)
	{
		static int nclients = 0;
		char recvData[256];	//设置接收数据包的缓冲区（假设要接收的信息不超过
							//255字节，再加串结束符）

		//接收UDP的数据包
		int ret = server.Receive(client, recvData, 256);
		if (ret > 0)
		{
			recvData[ret] = '\0';		//填充字符串结束符'\0'
			printf("客户端%s发来消息：", client.GetIP());
			printf(recvData);			//输出接收到的消息          
		}

		nclients++;						//已服务的客户端数目加1
		char sendData[256];

		//构造要发送的数据包
		sprintf(sendData, "您好，第%03d号客户，很高兴为您提供服务。\n", nclients);

		//发送数据包
		server.Send(client, sendData, 256);
	}

	return argc;
}