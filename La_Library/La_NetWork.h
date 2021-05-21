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
*  简    介: 
*
***************************************************************************************
*/
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma once
#include <winsock2.h>
#include "La_Windows.h"


#pragma comment(lib, "Urlmon.lib")
#pragma comment(lib, "Ws2_32.lib")
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

class DHOST
{
public:
	enum ADDRESSTYPE:int
	{
		IPv4 = AF_INET,
		IPv6 = AF_INET6,
		Bios = AF_NETBIOS,
	};
private:
	int numAddress;
	int numAlias;
	ARRAY<ASTRING> IPList;
	hostent* destHost;
protected:
	bool DealSearch(void);
public:
	DHOST(void) :destHost(nullptr), numAddress(0), numAlias(0) {};
	bool SearchHostByName(const char* name = nullptr);
	//要用在服务器当中才可以反向解析，电脑没有名字服务器查询PTR记录
	bool SearchHostByAddress(const char* address);
	// If the user input is an alpha name for the host, use gethostbyname()
	// If not, get host by addr (assume IPv4)
	bool IsHostName(const char* message) { return isalpha(message[0]); }
	ASTRING GetLocalName(void)//char* 不能返回指针啊，指针指向的其实是 Array 的 list ，没有匿名对象很快消失
	{
		ASTRING hostName(MAX_BUFFER);
		int result = gethostname(hostName, sizeof(hostName));
		if (result != 0)
		{
			MessageWarn(TEXT("获取本地主机名失败：%d\n"), result);
		}
		return hostName;
	}
	char* GetOfficialName(void)const { return destHost->h_name; }
	char** GetAliasList(void)const { return destHost->h_aliases; }
	char* GetAlias(int i)const { assert(i >= 0 && i < numAlias); return destHost->h_aliases[i]; }
	int GetAliasNumber(void)const { return numAlias; }
	//address 的处理是因为可能不是 PV4
	short GetAddressType(void)const { return destHost->h_addrtype; }
	short GetAddressLength(void)const { return destHost->h_length; }
	int  GetAddressNumber(void)const { return numAddress; }
	char** GetAddressList(void)const { return destHost->h_addr_list; }
	ARRAY<ASTRING> GetIPList(void)const { return IPList; }
	char* GetIP(int i) { assert(i >= 0 && i < IPList.GetSize()); return IPList[i]; }
	const char* GetIP(int i)const { assert(i >= 0 && i < IPList.GetSize()); return IPList[i]; }
	int GetIPNumber(void)const { return IPList.GetSize(); }
	char* AddressToIP(char* addressListN)
	{
		struct in_addr addr;
		addr.s_addr = *(u_long*)addressListN;
		return inet_ntoa(addr);
	}
};


class MSGPV4 
{
private:
	bool bConnect;//也可用来判断是否初始化
	SOCKET hSocket;
	sockaddr_in addrMsg;
public:
	MSGPV4() :addrMsg{ 0 }, bConnect(false), addrSize(sizeof(sockaddr_in)){};
	int addrSize;
	//nullptr 表示任何都可以
	void Set(const char* IP, int Port)
	{
		//填充相关地址信息，绑定IP地址、协议及端口
		//使用TCP/IP协议栈
		addrMsg.sin_family = AF_INET;
		//将端口转换为网络字节序
		addrMsg.sin_port = htons(Port);
		//绑定本地IP地址，可使用本机的任何IP地址。
		addrMsg.sin_addr.S_un.S_addr = IP ? inet_addr(IP) : INADDR_ANY;
	}
	//不知道为什么不可以
	////重载类型转化
	//operator sockaddr* () { return (sockaddr*)((sockaddr_in*)this); }
	//operator const sockaddr* const() { return (sockaddr*)((sockaddr_in*)this); }
	sockaddr* GetMsg(void)const { return (sockaddr*)&addrMsg; }
	char* GetIP(void)const { return inet_ntoa(addrMsg.sin_addr); }
};

class DSOCKET
{
protected:
	bool bCreate;
	SOCKET hSocket;
	void Close()
	{
		if (bCreate)
		{
			closesocket(hSocket);
		}
	}
public:
	DSOCKET() :bCreate(false), hSocket(INVALID_SOCKET) {};
	virtual bool Create(const char* destIP, const int destPort) = 0;
	virtual int Send(const MSGPV4& dest, const char* msg, int msgLength) = 0;
	//会获得 dest 的信息
	virtual int Receive(MSGPV4& dest, char* msg, int msgLength) = 0;
	~DSOCKET() { Close(); }
};


class UDP :public DSOCKET
{
public:
	virtual bool Create(const char* destIP, const int destPort)
	{
		if (!bCreate)
		{
			hSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

			if (hSocket == INVALID_SOCKET)
			{
				MessageWarn(TEXT("Get a handle to socket Failed!"));
				return false;
			}
			bCreate = true;
		}
		return true;
	}
	virtual int Send(const MSGPV4& dest, const char* msg, int msgLength)
	{
		return sendto(hSocket, msg, msgLength, 0, dest.GetMsg(), sizeof(sockaddr));
	}
	virtual int Receive(MSGPV4& dest, char* msg, int msgLength)
	{
		return recvfrom(hSocket, msg, msgLength, 0, dest.GetMsg(), &dest.addrSize);
	}
};

typedef UDP		UDPCLIENT;

class UDPSEVER :public UDP
{
public:
	//选择本机的IP地址用来通信，本机不止一个IP
	virtual bool Create(const char* sourceIP, const int sourcePort)
	{
		if (UDP::Create(nullptr, sourcePort))
		{
			MSGPV4 serverMsg;
			serverMsg.Set(sourceIP, sourcePort);
			//绑定socket套接字与地址
			if (bind(hSocket, serverMsg.GetMsg(), sizeof(sockaddr_in)) == SOCKET_ERROR)
			{
				MessageWarn(TEXT("绑定地址错误！"));
				Close();

				return false;
			}
			//不是双等于
			return bCreate = true;
		}
		return false;
	}
};


class SOCKET_MASTER
{
private:
	static int life;
	WSADATA wsaData;
public:
	bool Create();
	void Release();
	SOCKET_MASTER(void);
	~SOCKET_MASTER() { Release(); }
};

