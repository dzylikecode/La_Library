#define _CRT_SECURE_NO_WARNINGS
#include "Platform.h"
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <process.h>

//û������256��Ҫ��Ȼ�޷���Ӧ���޷����ܵ�
const int dataBuffer = 256;
UDPSEVER server;
ASTRING recvData(dataBuffer);
bool bRun = true;
MSGPV4 client;

//������Ϣ�ĺ���������һ���߳�������
void receive_msg(void* message = nullptr)
{
	
	while (1)
	{
		//������������Է�����Ϣ
		int ret = server.Receive(client, recvData, dataBuffer);
		if (ret > 0)
		{
			recvData[ret] = '\0';					//����ַ���������'\0'
			printf("����%s������Ϣ��%s\n", client.GetIP(), recvData);

			//�����Ϣ��bye�����˳�����
			if (recvData == "bye")
			{
				bRun = false;
				printf("ϵͳ�˳���");

				return;
			}
		}
	}
}

int main(int argc, char* argv[])
{
	server.Create(nullptr, 6666);

	//����������Ϣ�߳�
	_beginthread(receive_msg, 0, nullptr);
	ASTRING sendData(dataBuffer);
	while (1)
	{
		if (!bRun)
		{
			break;
		}

		//�Ӽ��̽���Ҫ���͵���Ϣ
		gets_s((char*)sendData, dataBuffer);
		//����������Ϣ
		server.Send(client, sendData, dataBuffer);

		//�����bye�����˳�����
		if (recvData == "bye")
		{
			bRun = false;
			printf("ϵͳ�˳���");
			break;
		}
	}

	//����������Ϣ���߳�
	_endthread();

	return argc;
}