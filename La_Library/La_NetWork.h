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
#include "La_WindowsBase.h"


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
	char* GetIP(int i) { assert(i >= 0 && i < IPList.getSize()); return IPList[i]; }
	const char* GetIP(int i)const { assert(i >= 0 && i < IPList.getSize()); return IPList[i]; }
	int GetIPNumber(void)const { return IPList.getSize(); }
	char* AddressToIP(char* addressListN)
	{
		struct in_addr addr;
		addr.s_addr = *(u_long*)addressListN;
		return inet_ntoa(addr);
	}
};

class DSOCKET
{
protected:
	bool bCreate;
	SOCKET hSocket;
	virtual void close()
	{
		if (bCreate)
		{
			closesocket(hSocket);
		}
	}
public:
	DSOCKET() :bCreate(false), hSocket(INVALID_SOCKET) {};
	SOCKET GetSocket()const { return hSocket; }
	~DSOCKET() { close(); }
};

class TCPSERVER;

class MSGPV4 
{
private:
	sockaddr_in addrMsg;
	bool bConnect;//Ҳ�������ж��Ƿ��ʼ��
	SOCKET hSocket;
public:
	friend class TCPSERVER;
	MSGPV4() :addrMsg{ 0 }, bConnect(false), addrSize(sizeof(sockaddr_in)){};
	MSGPV4(const char* IP, int Port) :bConnect(false), addrSize(sizeof(sockaddr_in))
	{
		Set(IP, Port);
	}
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
	int   GetPort()const { return ntohs(addrMsg.sin_port); }
	friend int  GetServerMSG(MSGPV4& server, SOCKET sourceSocket)
	{
		return getsockname(sourceSocket, server.GetMsg(), &server.addrSize);
	}
	friend int  GetClientMSG(MSGPV4& client, SOCKET sourceSocket)
	{
		return getpeername(sourceSocket, client.GetMsg(), &client.addrSize);
	}
	void close() { if (bConnect) { closesocket(hSocket); bConnect = false; } }
	SOCKET GetSocket()const { return hSocket; }
	~MSGPV4() { close(); }
};



//��ӵĹ����ǣ�ֻ�ܴ���һ�Σ������Ҫ�����趨������� close ������ create

class UDP :public DSOCKET
{
public:
	virtual bool create()
	{
		if (!bCreate)
		{
			hSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

			if (hSocket == INVALID_SOCKET)
			{
				MessageWarn(TEXT("Get a handle to socket Failed!"));
				return false;
			}
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

class UDPSERVER :public UDP
{
public:
	//ѡ�񱾻���IP��ַ����ͨ�ţ�������ֹһ��IP
	virtual bool create(const char* localIP, const int localPort)
	{
		if (UDP::create())
		{
			if (bCreate)
				return true;
			MSGPV4 localPV4;
			localPV4.Set(localIP, localPort);
			//��socket�׽������ַ
			if (bind(hSocket, localPV4.GetMsg(), sizeof(sockaddr_in)) == SOCKET_ERROR)
			{
				MessageWarn(TEXT("�󶨵�ַ����"));
				close();

				return false;
			}
			return true;
		}
		return false;
	}
};



class TCP:public DSOCKET
{
public:
	virtual bool create()
	{
		if (!bCreate)
		{
			//����socket�׽��֣�ָ��ʹ��TCPЭ��
			hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (hSocket == INVALID_SOCKET)
			{
				MessageWarn(TEXT("����socketʧ�ܣ�"));

				return false;
			}
		}
		return true;
	}
};

class TCPCLIENT :public TCP
{
public:
	virtual bool create(const char* serverIP, const int serverPort)
	{
		if (TCP::create())
		{
			if (bCreate)
				return true;
			MSGPV4 server(serverIP, serverPort);

			if (connect(hSocket, server.GetMsg(), server.addrSize) == SOCKET_ERROR)
			{
				MessageWarn(TEXT("���ӷ�����ʧ�ܣ�"));
				close();
				return false;
			}
			return true;
		}

		return false;
	}
	virtual int Send(const char* msg, int msgLength)
	{
		return send(hSocket, msg, msgLength, 0);
	}
	virtual int Receive(char* msg, int msgLength)
	{
		return recv(hSocket, msg, msgLength, 0);
	}
};


class TCPSERVER : public TCP
{
public:
	virtual bool create(const char* localIP, const int localPort, int maxSequence = 6)
	{
		if (TCP::create())
		{
			if (bCreate)
				return true;

			MSGPV4 localPV4(localIP, localPort);

			if (bind(hSocket,localPV4.GetMsg(), localPV4.addrSize) == SOCKET_ERROR)
			{
				MessageWarn(TEXT("�󶨵�ַʧ�ܣ�"));
				close();
				return false;
			}

			//��ʼ����
			//�����Ӷ��е���󳤶ȡ��������ΪSOMAXCONN
			if (listen(hSocket, maxSequence) == SOCKET_ERROR)
			{
				MessageWarn(TEXT("����ʧ�ܣ�"));
				close();
				return false;
			}
			return true;
		}
		return false;
	}
	virtual bool GetClient(MSGPV4& client)
	{
		//���û�У���һֱ�ȴ�
		client.hSocket = accept(hSocket, client.GetMsg(), &client.addrSize);
		if (client.hSocket == INVALID_SOCKET)
		{
			MessageWarn(TEXT("���ܿͻ������Ӵ���"));
			return false;
		}
		client.bConnect = true;
		return true;
	}
	virtual int Send(MSGPV4& client, const char* msg, int msgLength)
	{
		return send(client.hSocket, msg, msgLength, 0);
	}
	virtual int Receive(MSGPV4& client, char* msg, int msgLength)
	{
		return recv(client.hSocket, msg, msgLength, 0);
	}
};


//�������Ϊ��̬��Ա������
class SOCKET_MASTER
{
private:
	static int life;
	WSADATA wsaData;
public:
	bool create();
	void Release();
	SOCKET_MASTER(void);
	~SOCKET_MASTER() { Release(); }
};

