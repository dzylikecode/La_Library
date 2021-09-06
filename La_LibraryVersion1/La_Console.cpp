#include "LA_Console.h"

CONSOLE consoleOut;

bool CONSOLE::create(const TCHAR* name)
{
	AllocConsole();

	if (!name) setTitle(TEXT("New Console"));
	else       setTitle(name);

	freopen_s(&stream, "conin$", "r+t", stdin);//�ض���������
	freopen_s(&stream, "conout$", "w+t", stdout);//�ض���������
	hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	return true;
}