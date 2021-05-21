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

class DSOCKET
{
public:
	enum ADDRESSTYPE:int
	{
		IPv4 = AF_INET,
		IPv6 = AF_INET6,
		Bios = AF_NETBIOS,
	};
private:
	WSADATA wsaData;
	int numAddress;
	int numAlias;
	ARRAY<ASTRING> IPList;
	hostent* destHost;
protected:
	bool Deal(void);
public:
	DSOCKET(void);
	bool SearchHostByName(const char* name = nullptr);
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

	~DSOCKET(void){ WSACleanup(); }
};


class SOCKET_MASTER
{
private:
	static int life;
public:
	bool Create();
	void Release();
	SOCKET_MASTER(void);
};

