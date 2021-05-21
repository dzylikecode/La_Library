#define _CRT_SECURE_NO_WARNINGS
#include "La_NetWork.h"
#include "La_Windows.h"
#include "La_Shell.h"
#include <stdio.h>
//int WINAPI WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd)
//{
//	DSOCKET desthost;
//	desthost.SearchHostBy();
//	
//	return 0;
//}

int main(int argc, char* argv[])
{
	//��֤�����в����ĺϷ���
	if (argc != 2)
	{
		printf("�÷���getbyname ������\n      getbyname localhost ��ѯ����������IP��ַ");
		return 1;
	}
	DSOCKET host;

	ASTRING hostname = argv[1];
	if (hostname == "localhost")
	{
		hostname = host.GetLocalName();
	}
	
	if (host.IsHostName(hostname))
	{
		printf("��������%s\n", hostname);
		host.SearchHostByName(hostname);
	}
	else
	{
		printf("��ַ����%s\n", hostname);
		host.SearchHostByAddress(hostname);
	}
	

	//�����ַ���ͺ͵�ַ����
	printf("��ѯ�����\n");
	printf("\t��������%s\n", host.GetOfficialName());

	for (int i = 0; i < host.GetAliasNumber(); i++)
	{
		printf("\t���� #%d��%s\n", i + 1, host.GetAlias(i));
	}

	printf("\t��ַ���ͣ�");
	switch (host.GetAddressType())
	{
	case DSOCKET::IPv4:
		printf("AF_INET\n");
		break;
	case DSOCKET::Bios:
		printf("AF_NETBIOS\n");
		break;
	default:
		printf("%d\n", host.GetAddressType());
		break;
	}
	printf("\t��ַ���ȣ�%d\n", host.GetAddressLength());

	//������ص���IPv4��ַ���������ѯ���Ľ��
	if (host.GetAddressType() == DSOCKET::IPv4)
	{
		for (int i = 0; i < host.GetAddressNumber(); i++)
		{
			printf("\tIP��ַ#%d��%s\n", i, host.GetIP(i));
		}
	}
	else if (host.GetAddressType() == DSOCKET::Bios)
	{
		printf("���ص���NETBIOS��ַ");
	}

	return argc;
}