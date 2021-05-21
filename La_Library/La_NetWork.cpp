#include "La_NetWork.h"



int SOCKET_MASTER::life = 0;

SOCKET_MASTER::SOCKET_MASTER(void)
{
	Create();
}

bool SOCKET_MASTER::Create()
{
	if (!life++)
	{
		//对Windows Sockets的动态链接库WS2_32.dll进行初始化,版本即为 2.2
		int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (result != 0)
		{
			MessageWarn(TEXT("WSAStartup失败：%d\n"), result);
			return false;
		}
	}
	return true;
}

void SOCKET_MASTER::Release()
{
	if (!--life)
	{
		WSACleanup();
	}
}

SOCKET_MASTER socketMaster;

bool DHOST::SearchHostByName(const char* name)
{
	//好像一般不用中文域名
//#ifdef UNICODE
//#define GetHostName GetHostNameW
//#elif
//#define GetHostName gethostname
//#endif
	ASTRING hostName = name;

	if (!name)
		hostName = GetLocalName();

	destHost = gethostbyname(hostName);


	return DealSearch();
}

bool DHOST::SearchHostByAddress(const char* address)
{
	struct in_addr addr;
	addr.s_addr = inet_addr(address);
	if (addr.s_addr == INADDR_NONE) 
	{
		MessageWarn(TEXT("The IPv4 address entered must be a legal address\n"));
		return false;
	}
	else
	{
		destHost = gethostbyaddr((char*)&addr, 4, AF_INET);
	}

	return DealSearch();
}

bool DHOST::DealSearch()
{
	if (!destHost)
	{
		DWORD errcode = WSAGetLastError();

		if (errcode == WSAHOST_NOT_FOUND)
		{
			MessageWarn(TEXT("主机没有找到！"));

			return false;
		}
		else if (errcode == WSANO_DATA)
		{
			MessageWarn(TEXT("没找到相应的数据记录！"));

			return false;
		}
		else
		{
			MessageWarn(TEXT("函数调用失败：%ld"), errcode);

			return false;
		}
	}

	while (destHost->h_aliases[numAlias])
		numAlias++;
	while (destHost->h_addr_list[numAddress])
		numAddress++;
	if (destHost->h_addrtype == IPv4)
	{
		IPList.Resize(numAddress);
		for (int i = 0; i < numAddress; i++)
		{
			IPList[i] = AddressToIP(destHost->h_addr_list[i]);
		}
	}

	return true;
}