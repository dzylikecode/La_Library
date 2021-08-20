#include <La_NetWork.h>
#include <La_Windows.h>
#include <La_Windows.h>
#include <La_Master.h>
#include <iostream>
#include <process.h>
#include <conio.h>

using namespace LADZY;
using namespace std;

#define N 10			//�����������

char* users[N] = { NULL };//�����û��б�
int nusers = 0;			//��ǰ��������
int index = 0;			//˽�Ķ���ѡ��
int startx = 2;			//��Ϣ��������
int isprivate = 0;		//�Ƿ���˽��״̬ 1�� 0��

//��ʾ��������û��б�
void showusers()
{
	consoleOut.setColor(B_BLACK | F_LOW_WITHTE);
	int k = 0;

	//��������û��б�
	for (int i = 0; i < N; i++)
	{
		consoleOut.gotoXY(68, 4 + k);
		if (users[i] != NULL)
		{
			printf("%5s     ", users[i]);
			k++;
		}
	}

	//���ʣ�µĿհ�
	for (int i = nusers; i < N; i++)
	{
		consoleOut.gotoXY(68, 4 + k);
		printf("%5s     ", " ");
		k++;
	}

	//��ʾ��ǰ��Ϣ���ն���
	if (!isprivate)
	{
		consoleOut.gotoXY(68, 23);
		printf("��������");

	}
}

//��������len���ַ���msg��������
void getInput(char* msg, int len)
{
	fflush(stdin);

	int t = 0;
	for (int i = 0; i < len; i++)
	{
		t = _getch();
		if (t == 9)	//Tab��
		{
			showusers();
			consoleOut.gotoXY(68, 4 + index);
			consoleOut.setColor(B_LOW_WITHTE);
			printf("%5s     ", users[index]);
			return;
		}
		if (t == 27)//ESC��
		{
			showusers();
			index = 0;
			consoleOut.gotoXY(68, 23);
			consoleOut.setColor(B_BLACK | F_LOW_WITHTE);
			printf("��������");
			consoleOut.setColor(B_LOW_WITHTE);
			consoleOut.gotoXY(15, 23);
			isprivate = 0;
			return;
		}
		if (t == 224)
		{
			t = _getch();
			if (t == 72)	//������ϼ�
			{
				index--;
				index = (index + nusers) % nusers;
			}
			if (t == 80)	//������¼�
			{
				index++;
				index = index % nusers;
			}
			showusers();
			consoleOut.gotoXY(68, 4 + index);
			consoleOut.setColor(B_LOW_WITHTE);
			printf("%5s     ", users[index]);
			return;
		}
		if (t != '\r' && t != '\n')	//����س���
		{
			putchar(t);
			msg[i] = t;
		}
		else
		{
			if (i > 0)
			{
				msg[i] = '\0';
				return;
			}
			else
			{
				consoleOut.gotoXY(68, 23);
				consoleOut.setColor(B_BLACK | F_LOW_WITHTE);
				printf("%8s", users[index]);
				isprivate = 1;		//����˽��״̬
				consoleOut.setColor(B_LOW_WITHTE);
				return;
			}
		}
	}
}

//�ڣ�x��y�������ƿ�width����height���ַ����ο�
void draw_box(int x, int y, int width, int height)
{
	consoleOut.gotoXY(x, y);
	printf("��");

	for (int i = 0; i < width; i++)
	{
		printf("��");
	}

	printf("��");

	for (int i = 1; i <= height; i++)
	{
		consoleOut.gotoXY(x, y + i);
		printf("��");

		consoleOut.gotoXY(x + width * 2 + 2, y + i);
		printf("��");
	}

	consoleOut.gotoXY(x, y + height + 1);
	printf("��");

	for (int i = 0; i < width; i++)
	{
		printf("��");
	}

	printf("��");
}

int drawui(void)
{
	char msg[100];

	consoleOut.resize(81, 26);
	consoleOut.hideCursor();
	consoleOut.clear();

	consoleOut.gotoXY(30, 1);
	printf("���������һ�ӭ�㣡");
	consoleOut.gotoXY(0, 4);
	draw_box(0, 2, 31, 19);	//������Ϣ�����ο�
	draw_box(66, 2, 5, 19);	//���������û��б������ο�

	consoleOut.gotoXY(68, 3);
	printf(" �����û�");

	showusers();			//��ʾ���������û��б�

	consoleOut.gotoXY(77, 1);
	printf("��");

	consoleOut.gotoXY(77, 23);
	printf("��");

	consoleOut.gotoXY(1, 23);
	printf("�����������ݣ�");

	consoleOut.setColor(B_LOW_WITHTE);
	printf("                                                  ");


	consoleOut.gotoXY(68, 23);
	consoleOut.setColor(B_BLACK | F_LOW_WITHTE);
	printf("��������");

	consoleOut.gotoXY(15, 23);

	return 0;
}

//����ͻ��˽����̣߳����ڽ��շ������˷����ĸ�����Ϣ
unsigned __stdcall receive_msg_thread(void* param)
{
	SOCKET clientsocket = *(SOCKET*)param;
	char message[81];	//��Ϣ������
	char name[81];		//���������Ļ�����
	int msglen;			//��Ϣ����
	int mtype;			//��Ϣ���ͣ�0-���� 1-˽�� 2-���� 3-���� 4-�����û��б�
	int i;				//��ʱ����

	while (1)
	{
		memset(message, 0, sizeof(message));///������Ϣ  

		msglen = recv(clientsocket, message, sizeof(message), 0);
		if (SOCKET_ERROR == msglen)
		{
			printf("������Ϣʧ�ܣ�������룺%d\n", WSAGetLastError());
			closesocket(clientsocket);
			WSACleanup();

			return 1;
		}

		message[msglen] = '\0';

		startx++;
		startx = startx % 22;
		if (startx == 0)
		{
			startx = 3;
			for (i = 3; i <= 21; i++)
			{
				consoleOut.setColor(B_BLACK | F_LOW_WITHTE);
				consoleOut.gotoXY(3, i);
				printf("%28s", "");
			}
		}
		consoleOut.gotoXY(3, startx);
		consoleOut.setColor(B_BLACK | F_LOW_WITHTE);

		mtype = message[0] - '0';

		if (mtype == 2)			//������Ϣ
		{
			int find = 0;		//�����Ƿ��Ѿ��������б���
			for (int k = 0; k < nusers; k++)
			{
				if (strcmp(users[k], message + 2) == 0)
				{
					find = 1;
					break;
				}
			}
			printf("%s�����ˣ�", message + 2);
			if (!find)			//������û�û�������û��б��У������
			{
				users[nusers] = (char*)malloc(strlen(message + 2) + 1);
				strcpy(users[nusers], message + 2);

				nusers++;
				for (int k = nusers; k < 10; k++)
				{
					users[k] = NULL;
				}
				showusers();
			}

			consoleOut.setColor(B_LOW_WITHTE | F_LOW_CYAN);
			consoleOut.gotoXY(15, 23);
			continue;
		}
		if (mtype == 3)			//�����û�������Ϣ
		{
			printf("%s�����ˣ�", message + 2);

			if (strcmp(message + 2, users[index]) == 0)//˽�������ߣ���Ϊ����
			{
				isprivate = 0;
			}

			for (i = 0; i < N; i++)
			{
				if ((users[i] != NULL) && (strcmp(message + 2, users[i]) == 0))
				{
					users[i] = NULL;
					break;
				}
			}

			nusers--;
			showusers();
			consoleOut.setColor(B_LOW_WITHTE | F_LOW_CYAN);
			consoleOut.gotoXY(15, 23);
			continue;
		}

		if (mtype == 4)			//���������û��б�
		{
			int n = 0;
			int j = 0;

			//������������û���Ϣ�����뵽�����û��б���
			for (i = 2; i < msglen; i++)
			{
				if (message[i] != ':')
				{
					name[j] = message[i];
					j++;
				}
				else
				{
					name[j] = '\0';
					if (users[n] != NULL)
					{
						free(users[n]);
					}
					users[n] = (char*)malloc(strlen(name) + 1);
					strcpy(users[n], name);
					n++;
					j = 0;
				}
			}
			nusers = n;
			for (i = nusers; i < N; i++)
			{
				if (users[i] != NULL)
				{
					free(users[i]);
				}
			}
			showusers();
			consoleOut.setColor(B_LOW_WITHTE | F_LOW_CYAN);
			consoleOut.gotoXY(15, 23);
			continue;
		}

		if (mtype == 0)			//��������Ϣ
		{
			printf("%s", message + 2);
			consoleOut.setColor(B_LOW_WITHTE | F_LOW_CYAN);
			consoleOut.gotoXY(15, 23);
			continue;
		}

		//����˽����Ϣ
		//��ȡ������
		int j = 0;
		for (i = 0; i < msglen; i++)
		{
			if (message[i] != ':')
			{
				name[j] = message[i];
				j++;
			}
			else
			{
				name[j] = '\0';
				break;
			}
		}

		printf("%s͵͵����˵��%s", name, message + i + 1);

		//���ص��������
		consoleOut.setColor(B_LOW_WITHTE | F_LOW_CYAN);
		consoleOut.gotoXY(15, 23);
	}
	return 1;
}

int main(void)
{
	consoleOut.create(TEXT("���������ң�"));
	SOCKET clientsocket;	//�ͻ����׽���


	clientsocket = CreateClientTCP();
	if (IsValidSocket(clientsocket))
	{
		printf("socket��ʼ��ʧ�ܣ��������: %d\n", WSAGetLastError());
		return -1;
	}

	//�����������������IP��ַ�������û���
	//���������IP��ַ  
	printf("�����������������IP��ַ��");
	char ip[81];
	memset(ip, 0, sizeof(ip));
	cin.getline(ip, 81);

	//����������û���  
	printf("�����������û�����");
	char name[81];
	memset(name, 0, sizeof(name));
	cin.getline(name, 81);

	MSG_IP msgIP;
	msgIP.createPV4(ip, 6789);

	printf("�����������������......\n");
	//���岽���������������  
	if (SOCKET_ERROR == Connect(clientsocket, msgIP))
	{
		printf("�������������ʧ�ܣ�������룺%d\n", WSAGetLastError());
		closesocket(clientsocket);

		return -1;
	}

	printf("�ɹ����ӵ������������IP��ַ��%s���˿ڣ�%d��\n\n", \
		msgIP.getIP(), msgIP.getPort());
	Sleep(500);
	//������������������Ϣ���߳�
	_beginthreadex(NULL, 0, &receive_msg_thread, (void*)&clientsocket, NULL, 0);

	//���߲������߳�������ѭ��
	char message[2048];
	char msg[100];
	int sendmsglen;

	//ƴ��������Ϣ
	char onlinemsg[81];
	strcpy(onlinemsg, "2:");
	strcat(onlinemsg, name);

	//���������ַ�����
	drawui();

	//�����û�������Ϣ
	send(clientsocket, onlinemsg, strlen(onlinemsg), 0);

	//�������������
	consoleOut.setColor(B_BLACK | F_LOW_WITHTE);
	consoleOut.gotoXY(30, 1);
	printf("���������һ�ӭ��%s��", name);

	//������ѭ��
	while (1)
	{
		consoleOut.setColor(B_LOW_WITHTE | F_LOW_CYAN);
		consoleOut.gotoXY(15, 23);

		memset(msg, '\0', sizeof(msg));
		getInput(msg, 50);
		//ģ�ⷢ�ͳ�ȥ�����������
		consoleOut.gotoXY(15, 23);
		printf("                                                  ");

		memset(message, 0, sizeof(message));
		if (!isprivate)
		{
			//���ģ���װ������Ϣ��0:������:��Ϣ
			strcpy(message, "0:");
			strcat(message, name);
			strcat(message, ":");
		}
		else
		{
			//˽�ģ���װ˽����Ϣ��1:������:������:��Ϣ
			strcpy(message, "1:");
			strcat(message, name);
			strcat(message, ":");
			strcat(message, users[index]);
			strcat(message, ":");
		}

		if (strlen(msg) > 0)
		{
			strcat(message, msg);

			//������Ϣ
			sendmsglen = send(clientsocket, message, strlen(message) + 1, 0);
			if (SOCKET_ERROR == sendmsglen)
			{
				printf("������Ϣʧ�ܣ�������룺%d\n", WSAGetLastError());
				closesocket(clientsocket);
				WSACleanup();

				return -1;
			}
		}
	}

	//�ڰ˲����ر��׽��֣����������Դ
	closesocket(clientsocket);

	return 0;
}
