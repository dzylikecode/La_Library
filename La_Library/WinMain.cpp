#define _CRT_SECURE_NO_WARNINGS
#include "Platform.h"
#include "La_File.h"
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <process.h>
const int MAX_SIZE = 10240;

int main(int argc, char* argv[])
{
	if (argc != 4)
	{
		printf("使用方式： upload 文件名 服务器IP地址 端口\n");
		exit(1);
	}

	TCPCLIENT client;
	client.Create(argv[2], atoi(argv[3]));


	STDFILE file;
	if (!file.Open(argv[1], "rb"))
	{
		printf("无法打开文件%s\n", argv[1]);

		exit(1);
	}
	char data[MAX_SIZE];
	int i;
	int ret;

	printf("发送文件名……\n");
	int result = client.Send(argv[1], strlen(argv[1]));
	result = client.Send("\0", 1);

	printf("发送文件内容");

	while (1)
	{
		memset((void*)data, 0, sizeof(data));
		i = fread(data, 1, sizeof(data), file.GetFile());
		if (i == 0)
		{
			printf("\n发送成功\n");
			break;
		}
		ret = client.Send(data, i);
		putchar('.');
		if (ret == SOCKET_ERROR)
		{
			printf("\n发送失败，文件可能不完整\n");
			break;
		}
	}
	return argc;
}

