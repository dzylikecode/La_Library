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
		printf("ʹ�÷�ʽ�� server �˿�\n");
		exit(1);
	}

	int port = atoi(argv[1]);
	
	//winsock2.h����#define SOMAXCONN	0x7fffffff /* (5) in WinSock1.1 */
	if (!tcpServer.Create("0.0.0.0", port, SOMAXCONN))
	{
		printf("����������ʧ�ܣ�\n");
	}
	printf("�������������������ڶ˿�%d��\n", port);

	MSGPV4 client; //Ӧ��˵�� temp һ��Ҫ������ȫ�ֵ��������ڣ� ����������
	while (1)
	{
		tcpServer.GetClient(client);

		printf("�ͻ��ˣ�%s��%d�������ӣ�\n", client.GetIP(), client.GetPort());

		//�����̴߳���һ���ͻ��˵��ϴ�
		_beginthread(DealClient, 0, (void*)&client);
		Sleep(1000);

		//���˫����IP��ַ���˿���Ϣ
		ReportConnectInfo(client.GetSocket());
	}

	while (numThread)
		Sleep(20);

	_endthread();
	return argc;
}

//�������˫����IP��ַ�Ͷ˿���Ϣ
void ReportConnectInfo(SOCKET threadMsg)
{
	MSGPV4 server;
	GetServerMSG(server, threadMsg);
	MSGPV4 client;
	GetClientMSG(client, threadMsg);
	printf("����������%s��%d��\n", server.GetIP(), server.GetPort());
	printf("�ͻ��ˣ���%s��%d��\n", client.GetIP(), client.GetPort());
}

//��Ҫ���붫�����������һЩ���
//�� client ����main �������� main ͬ����
//���Ƿ��� while ѭ����ÿ�� client ����ر� socket
//�������������̲߳�û��ʲô��
//���� client �����������
//���൱���� client ���� socket ������� client ����ʱ��Ӧ��ע��
//���������Ҫǳ���ƣ���Ϊ socket ����һ��ָ��
//����ͻ����ļ��ϴ����߳�
void DealClient(void* param)
{
	numThread++;
	//param �����ֵ��Ҫ�������������������ã���Ϊ param ��ʱ���ڱ�
	MSGPV4 client = *(MSGPV4*)param;
	printf("\n\n\n�������̳߳ɹ���\n\n\n");
	char filename[MAX_PATH];
	char data[MAX_SIZE];
	char ch;
	STDFILE file;
	printf("�����ļ���\n");

	memset((void*)filename, 0, sizeof(filename));
	int i;
	for (i = 0; i < sizeof(filename); i++)
	{
		if (tcpServer.Receive(client, &ch, 1) != 1)
		{
			printf("����ʧ�ܻ�ͻ����ѹر�����\n");
			numThread--;
			return;
		}
		if (ch == 0)
		{
			break; //������ filename һ��ʼ����յģ��ʲ��ø��� \0
		}
		filename[i] = ch;
	}
	if (i == sizeof(filename))
	{
		printf("�ļ�������\n");
		numThread--;
		return;
	}
	printf("�ļ���%s\n", filename);

	if (!file.Open(filename, "wb"))
	{
		printf("�޷���д��ʽ���ļ�\n");
		numThread--;
		return;
	}
	printf("�����ļ�����");
	memset((void*)data, 0, sizeof(data));
	while (1)
	{
		//receive �ǻ�������
		int i = tcpServer.Receive(client, data, sizeof(data));
		putchar('.');//������ʾ����
		if (i == SOCKET_ERROR)
		{
			printf("\n����ʧ�ܣ��ļ����ܲ�����\n");
			break;
		}
		else if (i == 0)
		{
			printf("\n���ճɹ�\n");
			break;
		}
		else
		{
			fwrite((void*)data, 1, i, file.GetFile());
		}
	}
	numThread--;
}