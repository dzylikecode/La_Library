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
	//Ҫ���ڷ��������вſ��Է������������û�����ַ�������ѯPTR��¼
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
	//address �Ĵ�������Ϊ���ܲ��� PV4
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
	bool bConnect;//Ҳ�������ж��Ƿ��ʼ��
	SOCKET hSocket;
	sockaddr_in addrMsg;
public:
	MSGPV4() :addrMsg{ 0 }, bConnect(false), addrSize(sizeof(sockaddr_in)){};
	int addrSize;
	//nullptr ��ʾ�κζ�����
	void Set(const char* IP, int Port)
	{
		//�����ص�ַ��Ϣ����IP��ַ��Э�鼰�˿�
		//ʹ��TCP/IPЭ��ջ
		addrMsg.sin_family = AF_INET;
		//���˿�ת��Ϊ�����ֽ���
		addrMsg.sin_port = htons(Port);
		//�󶨱���IP��ַ����ʹ�ñ������κ�IP��ַ��
		addrMsg.sin_addr.S_un.S_addr = IP ? inet_addr(IP) : INADDR_ANY;
	}
	//��֪��Ϊʲô������
	////��������ת��
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
	//���� dest ����Ϣ
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
	//ѡ�񱾻���IP��ַ����ͨ�ţ�������ֹһ��IP
	virtual bool Create(const char* sourceIP, const int sourcePort)
	{
		if (UDP::Create(nullptr, sourcePort))
		{
			MSGPV4 serverMsg;
			serverMsg.Set(sourceIP, sourcePort);
			//��socket�׽������ַ
			if (bind(hSocket, serverMsg.GetMsg(), sizeof(sockaddr_in)) == SOCKET_ERROR)
			{
				MessageWarn(TEXT("�󶨵�ַ����"));
				Close();

				return false;
			}
			//����˫����
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

