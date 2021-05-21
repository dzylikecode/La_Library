/*
***************************************************************************************
*  ��    ��: 
*
*  ��    ��: LaDzy
*
*  ��    ��: mathbewithcode@gmail.com
*
*  ���뻷��: Visual Studio 2019
*
*  ����ʱ��: 2021/05/20 23:15:13
*  ����޸�: 
*
*  ��    ��: 
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
 * ���ļ�winsock2.hͷ�ļ��У�
 * struct  hostent {				//host entry�ṹ�壬��¼������������Ϣ��
 *	  char	*h_name;				//��ַ����ʽ������
 *	  char	**h_aliases;			//������������һ����NULL�����ı�������
 *	  short	h_addrtype;				//��ַ���ͣ�ͨ����AF_INET
 *	  short	h_length;				//��ַ���ֽڳ���
 *	  char	**h_addr_list;			//һ���������ֽ�˳���ʾ����NULL����
									//��������ַ�б�
 * #define h_addr h_addr_list[0]	//�����б���ĵ�һ��ΪĬ��������ַh_addr
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
	ASTRING GetLocalName(void)//char* ���ܷ���ָ�밡��ָ��ָ�����ʵ�� Array �� list ��û����������ܿ���ʧ
	{
		ASTRING hostName(MAX_BUFFER);
		int result = gethostname(hostName, sizeof(hostName));
		if (result != 0)
		{
			MessageWarn(TEXT("��ȡ����������ʧ�ܣ�%d\n"), result);
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

