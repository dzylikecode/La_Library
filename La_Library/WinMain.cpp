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

	//����Ҫ���͵����ݰ�
	const char* sendData = "��ã�TCP�����������ǿͻ��ˡ�\n";

	client.Send(sendData, strlen(sendData));

	char recvData[256];
	int ret = client.Receive(recvData, 256);
	if (ret > 0)
	{
		recvData[ret] = '\0';					//����ַ���������'\0'
		printf("��������Ӧ��%s", recvData);		//����յ������ݰ���Ϣ
	}

	return argc;
}