#include "La_NetWork.h"

namespace
{
	WSADATA wsaData;
}

namespace LADZY
{
	bool InitializeNetWork()
	{
		//对Windows Sockets的动态链接库WS2_32.dll进行初始化,版本即为 2.2
		int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (result != 0)
		{
			MessageWarn(TEXT("WSAStartup失败：%d\n"), result);
			return false;
		}
		return true;
	}
	bool CloseNetWork()
	{
		WSACleanup();
		return true;
	}
}

