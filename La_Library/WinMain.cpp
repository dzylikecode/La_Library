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
	//验证命令行参数的合法性
	if (argc != 2)
	{
		printf("用法：getbyname 主机名\n      getbyname localhost 查询本地主机的IP地址");
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
		printf("主机名：%s\n", hostname);
		host.SearchHostByName(hostname);
	}
	else
	{
		printf("地址名：%s\n", hostname);
		host.SearchHostByAddress(hostname);
	}
	

	//输出地址类型和地址长度
	printf("查询结果：\n");
	printf("\t主机名：%s\n", host.GetOfficialName());

	for (int i = 0; i < host.GetAliasNumber(); i++)
	{
		printf("\t别名 #%d：%s\n", i + 1, host.GetAlias(i));
	}

	printf("\t地址类型：");
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
	printf("\t地址长度：%d\n", host.GetAddressLength());

	//如果返回的是IPv4地址，则输出查询到的结果
	if (host.GetAddressType() == DSOCKET::IPv4)
	{
		for (int i = 0; i < host.GetAddressNumber(); i++)
		{
			printf("\tIP地址#%d：%s\n", i, host.GetIP(i));
		}
	}
	else if (host.GetAddressType() == DSOCKET::Bios)
	{
		printf("返回的是NETBIOS地址");
	}

	return argc;
}