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
		//��Windows Sockets�Ķ�̬���ӿ�WS2_32.dll���г�ʼ��,�汾��Ϊ 2.2
		int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (result != 0)
		{
			MessageWarn(TEXT("WSAStartupʧ�ܣ�%d\n"), result);
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
	//����һ�㲻����������
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
			MessageWarn(TEXT("����û���ҵ���"));

			return false;
		}
		else if (errcode == WSANO_DATA)
		{
			MessageWarn(TEXT("û�ҵ���Ӧ�����ݼ�¼��"));

			return false;
		}
		else
		{
			MessageWarn(TEXT("��������ʧ�ܣ�%ld"), errcode);

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