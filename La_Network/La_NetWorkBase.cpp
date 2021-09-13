#include "La_NetWorkBase.h"


#pragma comment(lib, "Urlmon.lib")
#pragma comment(lib, "Ws2_32.lib")
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
			/*MessageWarn(TEXT("WSAStartup失败：%d\n"), result);*/
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

