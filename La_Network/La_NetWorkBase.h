/*
***************************************************************************************
*  程    序:
*
*  作    者: LaDzy
*
*  邮    箱: mathbewithcode@gmail.com
*
*  编译环境: Visual Studio 2019
*
*  创建时间: 2021/05/20 23:15:13
*  最后修改:
*
*  简    介: 简洁易用是非常重要的
*
* 客户端的套接字可以不绑定自身的地址/端口，
* 因为udp在创建套接字后直接使用sendto(),
* 隐含操作是，在发送数据之前操作系统会为该套接字随机分配一个合适的udp端口，
* 将该套接字和本地地址信息绑定。
* 但是，如果服务器程序就绪后一上来就要发送数据给客户端，
* 那么服务器就需要知道客户端的地址信息和端口，
* 那么就不能让客户端的地址信息和端口号由客户端所在操作系统分配，
* 而是要在客户端程序指定了。怎么指定，那就是用bind()函数
*
***************************************************************************************
*/
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma once
#include <windows.h>



inline bool DownLoad(const TCHAR* destFileName, const TCHAR* souceAddress)
{
	if (URLDownloadToFile(nullptr, souceAddress, destFileName, 0, nullptr) == S_OK)
	{
		return true;
	}
	return false;
}

/*
 *
 * 在文件winsock2.h头文件中：
 * struct  hostent {				//host entry结构体，记录主机的如下信息：
 *	  char	*h_name;				//地址的正式主机名
 *	  char	**h_aliases;			//主机别名，是一个以NULL结束的别名数组
 *	  short	h_addrtype;				//地址类型，通常是AF_INET
 *	  short	h_length;				//地址的字节长度
 *	  char	**h_addr_list;			//一个以网络字节顺序表示的以NULL结束
									//的主机地址列表
 * #define h_addr h_addr_list[0]	//定义列表项的第一项为默认主机地址h_addr
 * };
 *
 */
namespace LADZY
{
	class HOST
	{
	public:
		hostent* destHost;
	public:
		HOST() :destHost(nullptr) {};
		bool searchByName(const char* name = nullptr)
		{
			const char* hostName = name;
			if (!hostName)
			{
				hostName = getLocalName();
			}
			destHost = gethostbyname(hostName);
			return destHost;
		}
		char* getLocalName()const
		{
			static char hostName[1024];
			int result = gethostname(hostName, sizeof(hostName));
			if (result != 0)
			{
				/*MessageWarn(TEXT("获取本地主机名失败：%d\n"), result);*/
				return nullptr;
			}
			return hostName;
		}
		bool searchByIP(const char* IP)
		{
			struct in_addr addr;
			addr.s_addr = inet_addr(IP);
			if (addr.s_addr == INADDR_NONE)
			{
				/*_tprintf(TEXT("The IPv4 address entered must be a legal address\n"));*/
				return false;
			}
			else
			{
				destHost = gethostbyaddr((char*)&addr, 4, AF_INET);
			}
			return destHost;
		}
	};
#define IsHostName isalpha
	inline char* AddressToIP(const char* address)
	{
		struct in_addr addr;
		addr.s_addr = *(u_long*)address;
		return inet_ntoa(addr);
	}
}


namespace LADZY
{
	inline SOCKET CreateServerUDP()
	{
		return socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	}
	inline SOCKET CreateClientUDP()
	{
		return socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	}
#define IsValidSocket(s)		(s == INVALID_SOCKET)
	class MSG_IP
	{
	public:
		struct sockaddr_in addr;
	public:
		void createPV4(const char* IP4, int port)
		{
			clear();
			addr.sin_family = AF_INET;
			addr.sin_port = htons(port);
			addr.sin_addr.S_un.S_addr = IP4 ? inet_addr(IP4) : htonl(INADDR_ANY);
		}
		char* getIP()const { return inet_ntoa(addr.sin_addr); }
		int getPort()const { return ntohs(addr.sin_port); }
		void clear()
		{
			memset((void*)&addr, 0, sizeof(addr));
		}
	};

	inline int Bind(SOCKET socketS, const MSG_IP& msgIP)
	{
		return bind(socketS, (struct sockaddr*)&(msgIP.addr), sizeof(msgIP.addr));
	}
	inline SOCKET CreateServerUDP(const char* IP4, int port)
	{
		SOCKET server = CreateServerUDP();
		if (IsValidSocket(server))
		{
			return INVALID_SOCKET;
		}
		MSG_IP serIP;
		serIP.createPV4(IP4, port);
		if (Bind(server, serIP) == SOCKET_ERROR)
		{
			closesocket(server);
			return INVALID_SOCKET;
		}
		return server;
	}
	inline int GetServerMsgFrom(SOCKET server, MSG_IP& whatGet)
	{
		int size = sizeof(whatGet.addr);
		return getsockname(server, (sockaddr*)&(whatGet.addr), &size);
	}
	inline int GetClientMsgFrom(SOCKET client, MSG_IP& whatGet)
	{
		int size = sizeof(whatGet.addr);
		return getpeername(client, (sockaddr*)&(whatGet.addr), &size);
	}


	inline int ReceiveUDP(SOCKET process, char* msg, int msgLen, MSG_IP* whatGet = nullptr, int flag = 0)
	{
		if (whatGet)
		{
			int size = sizeof(whatGet->addr);
			return recvfrom(process, msg, msgLen, flag, (sockaddr*)&(whatGet->addr), &size);
		}

		return recvfrom(process, msg, msgLen, flag, nullptr, nullptr);
	}

	inline int SendUDP(SOCKET process, const char* msg, int msgLen, MSG_IP* destIP, int flag = 0)
	{
		int size = sizeof(destIP->addr);
		return sendto(process, msg, msgLen, flag, (sockaddr*)&(destIP->addr), size);
	}


}


namespace LADZY
{
	inline SOCKET CreateServerTCP()
	{
		return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	}

	inline SOCKET CreateClientTCP()
	{
		return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	}
	inline int Connect(SOCKET process, const MSG_IP& dest)
	{
		return connect(process, (struct sockaddr*)&(dest.addr), sizeof(dest.addr));
	}
	inline SOCKET CreateServerTCP(const char* IP4, int port, int num = 6)
	{
		SOCKET server = CreateServerTCP();
		if (IsValidSocket(server))
		{
			return INVALID_SOCKET;
		}
		MSG_IP serIP;
		serIP.createPV4(IP4, port);
		if (Bind(server, serIP) == SOCKET_ERROR)
		{
			closesocket(server);
			return INVALID_SOCKET;
		}
		if (listen(server, num) == SOCKET_ERROR)
		{
			closesocket(server);
			return INVALID_SOCKET;
		}
		return server;
	}

	inline SOCKET CreateClientTCP(const char* destIP4, int destPort)
	{
		SOCKET client = CreateClientTCP();
		MSG_IP dest;
		dest.createPV4(destIP4, destPort);
		if (Connect(client, dest))
		{
			closesocket(client);
			return INVALID_SOCKET;
		}
		return client;
	}

	inline SOCKET Accept(SOCKET process, MSG_IP& whatGet)
	{
		int size = sizeof(whatGet.addr);
		return accept(process, (SOCKADDR*)&(whatGet.addr), &size);
	}

	inline int ReceiveTCP(SOCKET process, char* msg, int msgLen, int flag = 0)
	{
		return recv(process, msg, msgLen, flag);
	}

	inline int SendTCP(SOCKET process, const char* msg, int msgLen, int flag = 0)
	{
		return send(process, msg, msgLen, flag);
	}


}

namespace LADZY
{
	bool InitializeNetWork();
	bool CloseNetWork();
}