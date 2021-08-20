#include <La_NetWork.h>
#include <La_Windows.h>
#include <La_Windows.h>
#include <La_Master.h>
#include <iostream>
#include <process.h>
#include <conio.h>

using namespace LADZY;
using namespace std;

#define N 10			//最大在线人数

char* users[N] = { NULL };//在线用户列表
int nusers = 0;			//当前在线人数
int index = 0;			//私聊对象选择
int startx = 2;			//信息区行坐标
int isprivate = 0;		//是否处于私聊状态 1是 0否

//显示输出在线用户列表
void showusers()
{
	consoleOut.setColor(B_BLACK | F_LOW_WITHTE);
	int k = 0;

	//输出在线用户列表
	for (int i = 0; i < N; i++)
	{
		consoleOut.gotoXY(68, 4 + k);
		if (users[i] != NULL)
		{
			printf("%5s     ", users[i]);
			k++;
		}
	}

	//输出剩下的空白
	for (int i = nusers; i < N; i++)
	{
		consoleOut.gotoXY(68, 4 + k);
		printf("%5s     ", " ");
		k++;
	}

	//显示当前信息接收对象
	if (!isprivate)
	{
		consoleOut.gotoXY(68, 23);
		printf("给所有人");

	}
}

//限制输入len个字符到msg缓冲区中
void getInput(char* msg, int len)
{
	fflush(stdin);

	int t = 0;
	for (int i = 0; i < len; i++)
	{
		t = _getch();
		if (t == 9)	//Tab键
		{
			showusers();
			consoleOut.gotoXY(68, 4 + index);
			consoleOut.setColor(B_LOW_WITHTE);
			printf("%5s     ", users[index]);
			return;
		}
		if (t == 27)//ESC键
		{
			showusers();
			index = 0;
			consoleOut.gotoXY(68, 23);
			consoleOut.setColor(B_BLACK | F_LOW_WITHTE);
			printf("给所有人");
			consoleOut.setColor(B_LOW_WITHTE);
			consoleOut.gotoXY(15, 23);
			isprivate = 0;
			return;
		}
		if (t == 224)
		{
			t = _getch();
			if (t == 72)	//光标向上键
			{
				index--;
				index = (index + nusers) % nusers;
			}
			if (t == 80)	//光标向下键
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
		if (t != '\r' && t != '\n')	//输入回车键
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
				isprivate = 1;		//进入私聊状态
				consoleOut.setColor(B_LOW_WITHTE);
				return;
			}
		}
	}
}

//在（x，y）处绘制宽width，高height的字符矩形框
void draw_box(int x, int y, int width, int height)
{
	consoleOut.gotoXY(x, y);
	printf("┏");

	for (int i = 0; i < width; i++)
	{
		printf("━");
	}

	printf("┓");

	for (int i = 1; i <= height; i++)
	{
		consoleOut.gotoXY(x, y + i);
		printf("┃");

		consoleOut.gotoXY(x + width * 2 + 2, y + i);
		printf("┃");
	}

	consoleOut.gotoXY(x, y + height + 1);
	printf("┗");

	for (int i = 0; i < width; i++)
	{
		printf("━");
	}

	printf("┛");
}

int drawui(void)
{
	char msg[100];

	consoleOut.resize(81, 26);
	consoleOut.hideCursor();
	consoleOut.clear();

	consoleOut.gotoXY(30, 1);
	printf("天天聊天室欢迎你！");
	consoleOut.gotoXY(0, 4);
	draw_box(0, 2, 31, 19);	//绘制消息区矩形框
	draw_box(66, 2, 5, 19);	//绘制在线用户列表区矩形框

	consoleOut.gotoXY(68, 3);
	printf(" 在线用户");

	showusers();			//显示所有在线用户列表

	consoleOut.gotoXY(77, 1);
	printf("↑");

	consoleOut.gotoXY(77, 23);
	printf("↓");

	consoleOut.gotoXY(1, 23);
	printf("输入聊天内容：");

	consoleOut.setColor(B_LOW_WITHTE);
	printf("                                                  ");


	consoleOut.gotoXY(68, 23);
	consoleOut.setColor(B_BLACK | F_LOW_WITHTE);
	printf("给所有人");

	consoleOut.gotoXY(15, 23);

	return 0;
}

//聊天客户端接收线程，用于接收服务器端发来的各种消息
unsigned __stdcall receive_msg_thread(void* param)
{
	SOCKET clientsocket = *(SOCKET*)param;
	char message[81];	//消息缓冲区
	char name[81];		//解析姓名的缓冲区
	int msglen;			//消息长度
	int mtype;			//消息类型：0-公聊 1-私聊 2-上线 3-离线 4-在线用户列表
	int i;				//临时变量

	while (1)
	{
		memset(message, 0, sizeof(message));///接收消息  

		msglen = recv(clientsocket, message, sizeof(message), 0);
		if (SOCKET_ERROR == msglen)
		{
			printf("接收消息失败，错误代码：%d\n", WSAGetLastError());
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

		if (mtype == 2)			//上线消息
		{
			int find = 0;		//查找是否已经在在线列表中
			for (int k = 0; k < nusers; k++)
			{
				if (strcmp(users[k], message + 2) == 0)
				{
					find = 1;
					break;
				}
			}
			printf("%s上线了！", message + 2);
			if (!find)			//如果该用户没在在线用户列表中，则加入
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
		if (mtype == 3)			//处理用户离线消息
		{
			printf("%s离线了！", message + 2);

			if (strcmp(message + 2, users[index]) == 0)//私聊人离线，改为公聊
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

		if (mtype == 4)			//处理在线用户列表
		{
			int n = 0;
			int j = 0;

			//逐个解析在线用户信息，加入到在线用户列表中
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

		if (mtype == 0)			//处理公聊消息
		{
			printf("%s", message + 2);
			consoleOut.setColor(B_LOW_WITHTE | F_LOW_CYAN);
			consoleOut.gotoXY(15, 23);
			continue;
		}

		//处理私聊消息
		//提取发信人
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

		printf("%s偷偷对你说：%s", name, message + i + 1);

		//光标回到输入框中
		consoleOut.setColor(B_LOW_WITHTE | F_LOW_CYAN);
		consoleOut.gotoXY(15, 23);
	}
	return 1;
}

int main(void)
{
	consoleOut.create(TEXT("天天聊天室！"));
	SOCKET clientsocket;	//客户端套接字


	clientsocket = CreateClientTCP();
	if (IsValidSocket(clientsocket))
	{
		printf("socket初始化失败，错误代码: %d\n", WSAGetLastError());
		return -1;
	}

	//第三步：输入服务器IP地址和聊天用户名
	//输入服务器IP地址  
	printf("请输入聊天服务器的IP地址：");
	char ip[81];
	memset(ip, 0, sizeof(ip));
	cin.getline(ip, 81);

	//输入聊天的用户名  
	printf("请输入聊天用户名：");
	char name[81];
	memset(name, 0, sizeof(name));
	cin.getline(name, 81);

	MSG_IP msgIP;
	msgIP.createPV4(ip, 6789);

	printf("正在连接聊天服务器......\n");
	//第五步：连接聊天服务器  
	if (SOCKET_ERROR == Connect(clientsocket, msgIP))
	{
		printf("连接聊天服务器失败，错误代码：%d\n", WSAGetLastError());
		closesocket(clientsocket);

		return -1;
	}

	printf("成功连接到聊天服务器（IP地址：%s，端口：%d）\n\n", \
		msgIP.getIP(), msgIP.getPort());
	Sleep(500);
	//第六步：启动接收消息的线程
	_beginthreadex(NULL, 0, &receive_msg_thread, (void*)&clientsocket, NULL, 0);

	//第七步：主线程中聊天循环
	char message[2048];
	char msg[100];
	int sendmsglen;

	//拼接上线消息
	char onlinemsg[81];
	strcpy(onlinemsg, "2:");
	strcat(onlinemsg, name);

	//绘制聊天字符界面
	drawui();

	//发送用户上线消息
	send(clientsocket, onlinemsg, strlen(onlinemsg), 0);

	//设置聊天标题区
	consoleOut.setColor(B_BLACK | F_LOW_WITHTE);
	consoleOut.gotoXY(30, 1);
	printf("天天聊天室欢迎你%s！", name);

	//聊天主循环
	while (1)
	{
		consoleOut.setColor(B_LOW_WITHTE | F_LOW_CYAN);
		consoleOut.gotoXY(15, 23);

		memset(msg, '\0', sizeof(msg));
		getInput(msg, 50);
		//模拟发送出去，发送栏清空
		consoleOut.gotoXY(15, 23);
		printf("                                                  ");

		memset(message, 0, sizeof(message));
		if (!isprivate)
		{
			//公聊，组装公聊消息：0:发信人:消息
			strcpy(message, "0:");
			strcat(message, name);
			strcat(message, ":");
		}
		else
		{
			//私聊，组装私聊消息：1:发信人:接收人:消息
			strcpy(message, "1:");
			strcat(message, name);
			strcat(message, ":");
			strcat(message, users[index]);
			strcat(message, ":");
		}

		if (strlen(msg) > 0)
		{
			strcat(message, msg);

			//发送消息
			sendmsglen = send(clientsocket, message, strlen(message) + 1, 0);
			if (SOCKET_ERROR == sendmsglen)
			{
				printf("发送消息失败，错误代码：%d\n", WSAGetLastError());
				closesocket(clientsocket);
				WSACleanup();

				return -1;
			}
		}
	}

	//第八步：关闭套接字，清理相关资源
	closesocket(clientsocket);

	return 0;
}
