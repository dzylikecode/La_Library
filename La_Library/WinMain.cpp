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
		char recvData[256];	//���ý������ݰ��Ļ�����������Ҫ���յ���Ϣ������
							//255�ֽڣ��ټӴ���������

		//����UDP�����ݰ�
		int ret = server.Receive(client, recvData, 256);
		if (ret > 0)
		{
			recvData[ret] = '\0';		//����ַ���������'\0'
			printf("�ͻ���%s������Ϣ��", client.GetIP());
			printf(recvData);			//������յ�����Ϣ          
		}

		nclients++;						//�ѷ���Ŀͻ�����Ŀ��1
		char sendData[256];

		//����Ҫ���͵����ݰ�
		sprintf(sendData, "���ã���%03d�ſͻ����ܸ���Ϊ���ṩ����\n", nclients);

		//�������ݰ�
		server.Send(client, sendData, 256);
	}

	return argc;
}