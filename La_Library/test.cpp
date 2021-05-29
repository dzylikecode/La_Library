#define  _WINSOCK_DEPRECATED_NO_WARNINGS

#include <string.h>
#include <process.h>
#include "Platform.h"

UDPCLIENT client;

MSGPV4 serveraddr;
char sendData[256];
char recvData[256];
int isrunning = 1;

//������Ϣ�ĺ���������һ���߳�������
void receive_msg(void* message = nullptr)
{
	while (1)
	{
		//������������Է�����Ϣ
		int ret = client.Receive(serveraddr, recvData, 256);
		if (ret > 0)
		{
			recvData[ret] = '\0';					//����ַ���������'\0'
			printf("����%s������Ϣ��%s\n", serveraddr.GetIP(), recvData);

			//�����Ϣ��bye�����˳�����
			if (!strcmp(recvData, "bye"))
			{
				isrunning = 0;
				printf("ϵͳ�˳���");

				return;
			}
		}
	}
}

int main(int argc, char* argv[])
{
	CONSOLE console;
	console.Create();
	console.SetTitle(TEXT("����UDP�ļ��������ͻ���"));
	console.resize(80, 25);
	console.Clear();
	printf("���������һ�ӭ�㣡\n\n");

	client.Create(nullptr, 6666);


	MSGPV4 server("127.0.0.1", 6666);

	//����������Ϣ�߳�
	_beginthread(receive_msg, 0, NULL);

	while (1)
	{
		if (!isrunning)
		{
			break;
		}

		//�Ӽ��̽���Ҫ���͵���Ϣ
		gets_s(sendData);
		//����������Ϣ
		client.Send(server, sendData, 256);


		//�����bye�����˳�����
		if (!strcmp(recvData, "bye"))
		{
			isrunning = 0;
			printf("ϵͳ�˳���");
			break;
		}
	}

	//����������Ϣ���߳�
	_endthread();

	return 0;
}
